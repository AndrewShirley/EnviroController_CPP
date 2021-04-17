#ifndef _ValueController_H
#define _ValueController_H


#include <Arduino.h>
#include <Time.h>
#include <stdlib.h>
#include <vector>


/*
    This is a generic value monitoring app, determining if its within a range.


    There are modes that this object will pass through:
    1) Nominal  - The value is within the range and has been for a while now.

    2) Active Control: Raising or Lowering - The value is outside the range and is currently being raised or lowered by the controlling software
        ie: The Heat is too high. Its exceeded 30C and currently the fan is on and the temperature is dropping
        Active controlling is split into "Active Lowering" and "Active Raising" which are broken out into 
        their own enums

    3) Idling mode - We have reached the target value and we're now in Idling Mode - which means this:
        if Approaching from below target (warming) then ignore the upper bounds for a few minutes. (ie: if heating, allows for the heat to rise above range and gently fall back within with triggering active cooling)
        if Approavhing from above the target (cooling) then ignore the lower bounds for a few minutes


    Various event messages are dispatched from this object back to your listener so that you can react accordingly.
    Messages are in JSON with the following general format:
    {
        EventType: Message,
        EventName: one of: ValueTooHigh, ValueTooLow, Nominal, TargetValueReached, IdlingHigh, IdlingLow
        Value: 27,              // 27C in this example

    
    }

    ValueTooHigh:
        The value has exceeded the NormalUpperRange value
        Value: [Current Value, NormalUpperRange, TargetValue]
    ValueTooLow:
        The value has dropped below the NormalLowerRange value
        Value: [Current Value, NormalLowerRange, TargetValue]
    Nominal:
        The value is within the range and there's no active raising or lowering happening
        Value: [CurrentValue, NormalLowerRange, NormalUpperRange]
    TargetValueReached:
        There was active lowering or raising and the Value is now equal or surpassed the TargetValue
        Value: [CurrentValue, TargetValue]


*/

struct ValueDataPoint
{
    float Value;
    unsigned long TimeStamp;
};

struct StatusResultStruct
{
    enum StatusResultEnum {ValueTooHigh, ValueTooLow, ValueInRange, TargetValueReached, EnteringNominalModeFromBelow, EnteringNominalModeFromAbove};
    char *StatusResultNames[6] = {"ValueTooHigh", "ValueTooLow", "ValueInRange", "TargetValueReached", "EnteringNominalModeFromBelow", "EnteringNominalModeFromAbove"};
    StatusResultEnum StatusResult=StatusResultEnum::ValueInRange;
};


class ValueController
{
    private:
        unsigned long NominalStartTime=0;           // Copy from millis() at the time we enter Nominal mode.
        unsigned long ActiveControllingStartTime=0; // Copy from millis() at the time the value leaves the range
        unsigned long IdlingStartTime=0;            // Copy from millis() at the time of starting idling

        ValueDataPoint **History;
        int HistoryPointer=-1;
        int MaxHistoryEntries=0, NumHistoryEntries=0;
        unsigned long  MaxIdlingTime = 0;                    // Set programically. Don't set this. Its copied from IdlingWaitToxxxxIntoRange

    public:
        bool MonitorLowerBounds = true;
        bool MonitorUpperBounds = true;

        enum Modes {Nominal, Raising, Lowering, Idling};
        char *ModeNames[4] = {"Nominal", "Raising", "Lowering", "Idling"};

        Modes CurrentControllingMode = Nominal;
        float TargetValue;              // Are we raising/lowering the value? If so, here's the target to reach
        float NormalLowerRange = 17;
        float NormalUpperRange = 30;
        float Raising_TargetValue = 27;
        float Lowering_TargetValue = 19;
        unsigned long IdlingWaitToFallIntoRange = 600;       // How long to wait for the Value to fall back into range while idling
        unsigned long IdlingWaitToRiseIntoRange = 600;       // How long to wait for the value to rise back into range while idling

    ValueController(int MaxHistorySize=1000)
    {
        History = (ValueDataPoint **)malloc( sizeof(ValueDataPoint) * MaxHistorySize);
        MaxHistoryEntries = MaxHistorySize;
    }

    ~ValueController()
    {
        if(History != nullptr)
            free(History);
    }



    // Internal use
    void AddValueToHistory(float Value)
    {
        HistoryPointer++;

        if(HistoryPointer >= MaxHistoryEntries)         // Loop past last array
        {
            HistoryPointer = 0;
        }
 
        NumHistoryEntries++;
        if(NumHistoryEntries >= MaxHistoryEntries)
        {
            NumHistoryEntries = MaxHistoryEntries;
        }

        ValueDataPoint * DP = new ValueDataPoint();
        DP->Value = Value;
        DP->TimeStamp = millis();
        History[HistoryPointer] = DP;
    }




    /*
        enum Modes {Nominal, Raising, Lowering, Idling};
        enum StatusResultEnum {ValueTooHigh, ValueTooLow, ValueInRange, TargetValueReached, EnteringNominalMode};

    */
    //StatusResultStruct *ProcessNewValue(StatusResultStruct &RS, float Value)
    void ProcessNewValue(StatusResultStruct &RS, float Value)
    {
        //StatusResultStruct *RS = new StatusResultStruct();
        
        bool NewValueAboveRange = Value > NormalUpperRange;
        bool NewValueBelowRange = Value < NormalLowerRange;

        RS.StatusResult=StatusResultStruct::StatusResultEnum::ValueInRange;

        switch (CurrentControllingMode){
            case Idling:                // From Idling, we can return to Nominal if the timelimit is exceeded. 
            {
                unsigned long TimeDelta = (millis() - IdlingStartTime) / 60;            // Convert to Seconds
                //Serial.printf(" TimeDelta=%ld MaxTime=%ld",TimeDelta,MaxIdlingTime);
                if(  (TimeDelta > MaxIdlingTime) ){          //  ||   (!NewValueAboveRange && !NewValueBelowRange) ){
                    NominalStartTime = millis();
                    if(CurrentControllingMode == Modes::Raising)
                        RS.StatusResult=StatusResultStruct::StatusResultEnum::EnteringNominalModeFromBelow;
                    else
                        RS.StatusResult=StatusResultStruct::StatusResultEnum::EnteringNominalModeFromAbove;
                    CurrentControllingMode = Nominal;
                }
            break;
            // No break on purpose... need it to fall through and reprocess CurrentControllingMode
            }
            case Nominal:       // From Nominal we can escalate to ActiveLowering and ActiveRaising
                if(NewValueAboveRange){
                    if(MonitorUpperBounds){
                        ActiveControllingStartTime = millis();
                        CurrentControllingMode = Lowering;
                        TargetValue = Lowering_TargetValue;
                        RS.StatusResult = StatusResultStruct::StatusResultEnum::ValueTooHigh;
                    }
                }else if(NewValueBelowRange)
                    if(MonitorLowerBounds)
                    {
                        ActiveControllingStartTime = millis();
                        CurrentControllingMode = Raising;
                        TargetValue = Raising_TargetValue;
                        RS.StatusResult = StatusResultStruct::StatusResultEnum::ValueTooLow;
                    }
                
                break;
            case Lowering:              // From Lowering we can reduce to Idling
                if(Value < TargetValue)
                {
                    IdlingStartTime = millis();
                    MaxIdlingTime = IdlingWaitToRiseIntoRange;
                    CurrentControllingMode = Idling;
                    RS.StatusResult = StatusResultStruct::StatusResultEnum::TargetValueReached;
                }
                else
                    RS.StatusResult = StatusResultStruct::StatusResultEnum::ValueTooHigh;
                break;
            case Raising:               // From Raising we can reduce to Idling
                if(Value > TargetValue)
                {
                    IdlingStartTime = millis();
                    MaxIdlingTime = IdlingWaitToFallIntoRange;
                    CurrentControllingMode = Idling;
                    RS.StatusResult = StatusResultStruct::StatusResultEnum::TargetValueReached;
                }
                else
                    RS.StatusResult = StatusResultStruct::StatusResultEnum::ValueTooLow;
                break;

            //return RS;
        }
        //return RS;
    }


};

#endif

