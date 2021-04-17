#ifndef _EnvironmentController_H
#define _EnvironmentController_H


#include <Arduino.h>
#include <Time.h>
#include <Adafruit_BME280.h>
#include "4_Channel_Relay.h"
#include "ValueController.h"


class EnvironmentController
{
    // Need:   BME280, 4-Channel Relay, GPIO Pin for Fan and humidifier

    private:
        bool InitializedMonitoredValues = false;
        Adafruit_BME280 *BME280=nullptr;
        Four_Channel_Relay *RelaySet = nullptr;
        unsigned long NextIntervalBeginsAt = 0;

        ValueController TemperatureController;
        ValueController HumidityController;

    public:
        float Temp_UpperRange = 31;
        float Temp_LowerRange = 19;
        float Temp_LoweringTargetValue = 22;                // Run fan until temp gets down to at least this

        float Hum_UpperRange = 57;
        float Hum_LowerRange = 48;
        float Hum_LoweringTargetValue = 50;                 // Run fan until humidity gets down to at least this
        float Hum_RaisingTargetValue = 55;                  // Run humidifier until humidity gets up to at least this



        int RelayChannel_Humidifier = 3;
        int RelayChannel_Fan = 0;
        unsigned long IntervalTime = 1;         // Ignore new readings until this much time has passed, no matter how much you call NEW READING()

    EnvironmentController(Adafruit_BME280 *BME280, Four_Channel_Relay *RelaySet, int RelayChannel_Hum=3, int RelayChannel_Fan=1)
    {
        this->BME280 = BME280;
        this->RelaySet = RelaySet;
        this->RelayChannel_Humidifier = RelayChannel_Hum;
        this->RelayChannel_Fan = RelayChannel_Fan;      

        this->TemperatureController.MonitorLowerBounds = false;     // Space heater monitors lower bounds
    }


    // Sets the Upper and lower ranges for monitored values Hum and Temp
    void SetMonitoredValues()
    {
        this->TemperatureController.NormalUpperRange = this->Temp_UpperRange;
        this->TemperatureController.NormalLowerRange = this->Temp_LowerRange;
        this->TemperatureController.Lowering_TargetValue = this->Temp_LoweringTargetValue;

        this->HumidityController.NormalUpperRange = this->Hum_UpperRange;
        this->HumidityController.NormalLowerRange = this->Hum_LowerRange;
        this->HumidityController.Lowering_TargetValue = Hum_LoweringTargetValue;                 // Run fan until humidity gets down to at least this
        this->HumidityController.Raising_TargetValue = Hum_RaisingTargetValue;                   // Run humidifier until humidity gets up to at least this

        this->InitializedMonitoredValues = true;
    }


    // Call this to take a reading and react to it
    void DoEverything()
    {
        float Temp, Hum;
        bool NewFanState = false, NewHumifierState = false;
        StatusResultStruct TempStatus, HumidityStatus;

        if(!this->InitializedMonitoredValues)
            this->SetMonitoredValues();

        this->BME280->takeForcedMeasurement();

        Temp = this->BME280->readTemperature();
        Hum = this->BME280->readHumidity();

        // Remove invalid readings coming from connection sensor issues
        if(Temp > -20 && Temp < 99)
            this->TemperatureController.ProcessNewValue(TempStatus, Temp);
        this->HumidityController.ProcessNewValue(HumidityStatus,Hum);

        // ValueTooHigh, ValueTooLow, ValueInRange, TargetValueReached, EnteringNominalModeFromBelow, EnteringNominalModeFromAbove
        // FanOn: Temp-ValueTooHigh, Hum-ValueTooHigh
        // HumOn: Hum-ValueTooLow

        // Should Fan be running?
        if(TempStatus.StatusResult == StatusResultStruct::ValueTooHigh ||
          HumidityStatus.StatusResult == StatusResultStruct::ValueTooHigh)
        {
            NewFanState = true;
        }
        if(HumidityStatus.StatusResult == StatusResultStruct::ValueTooLow)
        { 
            NewHumifierState = true;
        }

        RelaySet->SetChannel(RelayChannel_Fan, NewFanState);
        RelaySet->SetChannel(RelayChannel_Humidifier, NewHumifierState);

        //Serial.printf("Hum=%f HumState=%s",Hum,HumidityStatus.StatusResultNames[HumidityStatus.StatusResult]);
        //Serial.printf("TempRange=%f to %f", TemperatureController.NormalLowerRange,TemperatureController.NormalUpperRange);
        //Serial.printf(" HumRange=%f to %f\n", HumidityController.NormalLowerRange,HumidityController.NormalUpperRange);
        //Serial.printf("Temperature: Value:%f Resulted in %s Now in Mode: %s :: Humidiy=%-.2f :: FANS Fan=%d Hum=%d\n",Temp,TempStatus.StatusResultNames[TempStatus.StatusResult], this->TemperatureController.ModeNames[this->TemperatureController.CurrentControllingMode], Hum ,NewFanState, NewHumifierState );

    }


    // Call this lots. Only actually does anything every IntervalTime seconds
    void Loop()
    {
        if(millis() < NextIntervalBeginsAt) return;

        NextIntervalBeginsAt += IntervalTime * 1000;       // Keep timing ridged to absolute time

        DoEverything();
    }

};


#endif

