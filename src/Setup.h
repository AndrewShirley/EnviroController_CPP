#ifndef _Setup_H
#define _Setup_H

#include <Arduino.h>
#include <iostream>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "Images/DontPanic.h"
#include "AiEsp32RotaryEncoder.h"
#include "4_Channel_Relay.h"
#include "GUI/GraphicObjectBase.h"
#include <Fonts/FreeSansBold9pt7b.h>
#include "GUI/GUI_Text.h"
#include "GUI/PanelHolder.h"
#include "ValueController.h"
#include "EnvironmentController.h"
#include "WifiConnector.h"
#include "GUI/Image.h"
#include "AndrewsMQTT.h"

#define RotaryA 34
#define RotaryB 35
#define RotarySwitch 13
#define Relay_Channel1 16
#define Relay_Channel2 17
#define Relay_Channel3 18
#define Relay_Channel4 19

namespace Globals
{

    Four_Channel_Relay MyRelaySet = Four_Channel_Relay();

    //const int Oled_SCLPin = 15;
    //const int Oled_SDAPin = 4;
    const int Oled_SCLPin = 22;
    const int Oled_SDAPin = 21;
    const int BME280_SCLPin = 15;
    const int BME280_SDAPin = 4;

    AiEsp32RotaryEncoder MyRotary(RotaryA, RotaryB, RotarySwitch, -1);

    TwoWire Oled_I2C = TwoWire(0);
    TwoWire BME280_I2C = TwoWire(1);

    Adafruit_BME280 BME280; // I2C
    Adafruit_SSD1306 Oled = Adafruit_SSD1306(128, 64, &Oled_I2C, 16);

    PanelHolder MyPanel;

    GUI_Text t1(5, 5, 75, 25, "TEMP");
    GUI_Text t2(5, 35, 75, 25, "HUM");
    PanelImage BackgroundImage(0,0,128,64, DontPanic);


    EnvironmentController EV = EnvironmentController(&BME280, &MyRelaySet);

    void TestValueController()
    {
        ValueController V;

        V.IdlingWaitToRiseIntoRange = 1;
        V.IdlingWaitToFallIntoRange = 4;

        float Values[] = {25, 25, 25, 27, 27, 28, 28, 29, 30, 31, 31, 30, 29, 25, 20, 17, 16, 15, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 25, 25, 25, 29, 30, 29, 28, 25, 25, 25, 25, 25, -1};
        int i;
        StatusResultStruct SR;

        Serial.printf("%f to %f\n", V.NormalLowerRange, V.NormalUpperRange);

        V.MonitorUpperBounds = false;

        for (i = 0; Values[i] != -1; i++)
        {
            V.ProcessNewValue(SR, Values[i]);
            Serial.printf("Value=%f Result=%d V Mode%d Target=%lu\n", Values[i], SR.StatusResult, V.CurrentControllingMode, V.TargetValue);
            //delete SR;
            delay(100);
        }
    }

    // Todo: Perhaps consider using a queue with structs showing button press times, helps do double-click counting
    volatile bool RotaryButtonPressed = true;
    volatile long unsigned LastButtonPressed = 0;

    void TestRelaySet()
    {
        int Last;
        int Delay;

        MyRelaySet.SetAllChannels(false);

        // Do one at a time
        Last = -1;
        Delay = 800;
        for (int n = 0; n < 4; n++)
        {
            for (int i = 0; i < 4; i++)
            {
                if (Last > -1)
                    MyRelaySet.SetChannel(Last, false);
                Last = i;

                MyRelaySet.SetChannel(i, true);
                delay(Delay);
            }
            MyRelaySet.SetAllChannels(false);
            Delay /= 2;
            delay(1000);
        }

        // Blink All
        for (int n = 0; n < 1; n++)
        {
            MyRelaySet.SetAllChannels(true);
            delay(500);
            MyRelaySet.SetAllChannels(false);
            delay(500);
        }

        for (int ZZZZ = 0; ZZZZ < 5; ZZZZ++)
        {
            // Blink Sets of two,   1/2  and 3/4
            MyRelaySet.SetAllChannels(false);
            for (int n = 0; n < 2; n++)
            {
                for (int z = 0; z < 2; z++)
                {
                    MyRelaySet.SetChannel(0, z);
                    MyRelaySet.SetChannel(1, z);
                    MyRelaySet.SetChannel(2, 1 - z);
                    MyRelaySet.SetChannel(3, 1 - z);
                    delay(300);
                    MyRelaySet.SetChannel(0, 1 - z);
                    MyRelaySet.SetChannel(1, 1 - z);
                    MyRelaySet.SetChannel(2, z);
                    MyRelaySet.SetChannel(3, z);
                    delay(300);
                }
            }

            // Blink Sets of two, 0/3 and 1/2
            MyRelaySet.SetAllChannels(false);
            for (int n = 0; n < 2; n++)
            {
                for (int z = 0; z < 2; z++)
                {
                    MyRelaySet.SetChannel(0, z);
                    MyRelaySet.SetChannel(3, z);
                    MyRelaySet.SetChannel(1, 1 - z);
                    MyRelaySet.SetChannel(2, 1 - z);
                    delay(300);
                    MyRelaySet.SetChannel(0, 1 - z);
                    MyRelaySet.SetChannel(3, 1 - z);
                    MyRelaySet.SetChannel(1, z);
                    MyRelaySet.SetChannel(2, z);
                    delay(300);
                }
            }
        }

        MyRelaySet.SetAllChannels(false);
    }

    void rotary_onButtonClick()
    {
        RotaryButtonPressed = true;
        LastButtonPressed = millis();
        //cout << "C";
    }

    AndrewsMQTT MyMQTT = AndrewsMQTT();

    WifiConnector MyWifi("Deepbot", "Shirley1");

    void Setup()
    {
        Serial.begin(115200);

        bool Connected = MyWifi.Connect();
        Serial.printf("\nWifi Connected:%d", Connected);
        if (Connected)
        {
            Serial.printf(" LocalIP:%s", WiFi.localIP().toString().c_str());
        }
        Serial.printf("\n");

        // Initialize the Relays
        MyRelaySet.Begin();
        MyRelaySet.SetAllChannels(false);

        // Initialize the Rotary Encoder
        MyRotary.begin();
        MyRotary.setup(
            [] { MyRotary.readEncoder_ISR(); },
            [] { rotary_onButtonClick(); });

        // Initialize the BME280 Temperature/Humidity/Pressure sensor
        BME280_I2C.begin(BME280_SDAPin, BME280_SCLPin, 3400000);
        if (!BME280.begin(0x76, &BME280_I2C))
            Serial.println("BME280 not found");
        else
            Serial.println("\nInitialized the BME sensor");

        // Initialize the OLED screen
        Oled_I2C.begin(Oled_SDAPin, Oled_SCLPin, 3400000);

        if (!Oled.begin(SSD1306_SWITCHCAPVCC, 0x3c))
            Serial.println("OLED Failed Initialization");
        else
            Serial.println("Initialized the OLED");

        Oled.clearDisplay();
        Oled.fillRect(0, 0, 128, 64, 1);

//        Oled.drawBitmap(0, 0, DontPanic, 128, 64, WHITE, BLACK);

        //    I2CScan(BME280_I2C);
        //    I2CScan(Oled_I2C);
        MyPanel.AddItem("Background", &BackgroundImage);
        MyPanel.AddItem("Temp", &t2);
        MyPanel.AddItem("Hum", &t1);
        
        BackgroundImage.Image = DontPanic;
        t1.setBorderType(GraphicObjectBase::BorderTypes::Rounded);
        t1.setFont(&FreeSansBold9pt7b);
        t1.AddKeypress(90, &t2);  // UP (turn rotary) goes to t2
        t1.AddKeypress(270, &t2); // DOWN (turn rotary) goes to t2
        //t1.setSelected(true);
        t1.SetUniqueID("T1");

        t2.setBorderType(GraphicObjectBase::BorderTypes::Rounded);
        t2.setFont(&FreeSansBold9pt7b);
        t2.AddKeypress(90, &t1);
        t2.AddKeypress(270, &t1);
        t2.SetUniqueID("T2");

        Oled.setFont(&FreeSansBold9pt7b); // Have to do this in order to get the getTextBounds function to operate correctly

        t1.HorizontalAlignment = GUI_Text::HAlignment::Centre;
        t2.HorizontalAlignment = GUI_Text::HAlignment::Centre;
        t1.VerticalAlignment = GUI_Text::VAlignment::Middle;
        t2.VerticalAlignment = GUI_Text::VAlignment::Middle;

        MyPanel.Draw(Oled);
        Oled.display();



        Serial.printf("Leaving Setup\n");
    }

    int FutureTime = 0;
    int AnimationTime = 0;
    long PreviousRotary = -999;
    long CurrentRotary = -999;
    int Debounce = 0;

}

#endif
