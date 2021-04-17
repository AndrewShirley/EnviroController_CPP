/*




*/

#include <Arduino.h>
#include <PubSubClient.h>>
#include "Setup.h"
#include "AndrewsMQTT.h"
#include "PubSubClient.h"
#define ScreenSaverTime 2000
#define ScreenOnTime 5000
#define MQTTSendStatusTime 5000

#include <ArduinoJson.h>


void SendMQTTStatus()
{
  DynamicJsonDocument doc(1024);

  doc["millis"] = millis();
  doc["clientid"] = Globals::MyMQTT.ClientID;
  doc["temperature"] =   Globals::BME280.readTemperature();
  doc["humidity"] = Globals::BME280.readHumidity();
  String S;
  serializeJson(doc,S);
  Globals::MyMQTT.MyMQTTClient.publish("SensorPack3/Output",S.c_str());

}


void MQTTCallback(char* topic, byte* payload, unsigned int length)
{
 
	Serial.println(topic);
	Serial.print("Message:");

	for (int i = 0; i < length; i++) {
		Serial.print((char)payload[i]);
	}

	Serial.println("");
}



void setup()
{
  Globals::Setup();
  Globals::MyMQTT.ClientID = String("SensorPack3");
  Globals::MyMQTT.MyMQTTClient.setCallback(MQTTCallback);
}


int FutureTime = 0;
int AnimationTime = 0;
int FramePeriod = 100;       // mS between animation frames. Try to match camera's frame rate or sub-multiple. Here its 15 frames/S
long PreviousRotary = -999;
long CurrentRotary = -999;
int Debounce = 0;

int OffsetY=1;
int Direction = 1;
int OffsetYDelta = 1;

int ScreenSaverTimer = millis() + ScreenOnTime;
int ScreenSaverOn = false;
long Timer_NextMQTTStatus=0;
void loop()
{
  unsigned long Millis = millis();
  int16_t Delta = 0;
  long Temp = Globals::MyRotary.readEncoder();

  Globals::EV.Loop();       // Give the environmental controller some CPU time
  Globals::MyMQTT.loop();   // Keep the MQTT client happy


  /* // Poll the Rotary
  // if (int Delta2 = Globals::MyRotary.encoderChanged()) // Only process the input if it changed
  // {

  //   if (Millis >= Debounce || Debounce == 0)
  //   {
  //     int Direction = 90;
  //     Delta = Globals::MyRotary.readEncoder();
  //     Debounce = Millis + 150; // debounce time in mS

  //     if (Delta2 < 0)
  //       Direction = 270;

  //     cout << "Delta=" << Delta << " Direction=" << Direction << endl;

  //     //Globals::MyPanel.HandleInput(Direction, 1, false);

  //     Globals::MyPanel.HandleInput(Direction, abs(Delta), Globals::RotaryButtonPressed);
  //   }
  // }

  // if (Globals::RotaryButtonPressed)
  // {
  //   Globals::MyPanel.HandleInput(0, 0, true);
  //   Globals::RotaryButtonPressed = false; // Handle the click and reset
  //   Serial.println("CLICK");
  // }  */


  if (Millis >= AnimationTime)
  {
    AnimationTime += FramePeriod;
    //Serial.printf("D=%d",OffsetYDelta);
    OffsetY += (OffsetYDelta * Direction);
    OffsetYDelta += 1;
    if(OffsetY > 63 || OffsetY < 0)
    {
      Direction *= -1;
      OffsetY += (OffsetYDelta * Direction);
      OffsetYDelta = 1;
    }


    //Serial.printf("OffsetY=%d Dir=%d Delta=%d\n",OffsetY, Direction, Delta);

    Globals::BackgroundImage.setOffsetY(OffsetY);


//    if(OffsetX > 127)
//      OffsetX = 127;
    // if(OffsetY > 63)
    //   OffsetY = 63;


    //Serial.printf("-18=%d -19=%d\n",analogRead(34), analogRead(35));
  }

  if (Millis >= FutureTime)
  {
    char buf[30];
    char buf2[30];

    FutureTime += 4000; // Keep the timing rigid, using millis() + 4000 resets the timing to use NOW as the basis for the next cycles rather than the original requester. This can result on wonky looking timing that keeps resetting
    Globals::BME280.takeForcedMeasurement();

    sprintf(buf, "%0.1fC", Globals::BME280.readTemperature());
    sprintf(buf2, "%0.1f%%", Globals::BME280.readHumidity());
    cout << buf << " " << buf2 << endl;

    Globals::t1.setText(buf);
    Globals::t2.setText(buf2);

    //Globals::MyMQTT.MyMQTTClient.publish("testers",buf);

    //FramePeriod += 2;
    //float fps = 1 / ((float)FramePeriod / 1000.0);
    //Serial.printf("Increasing Frame rate to %d mS/Frame = %.2f frames/S\n",FramePeriod, fps);
    //Globals::Oled.

  }

  if(Millis > Timer_NextMQTTStatus){
    Timer_NextMQTTStatus += MQTTSendStatusTime;
    SendMQTTStatus();
  }

  bool ForceDraw = false;
  if(Millis > ScreenSaverTimer)
  {
    ScreenSaverOn = !ScreenSaverOn;
  
    if(ScreenSaverOn)
    {
      Globals::Oled.clearDisplay();
      Globals::Oled.display();
      ScreenSaverTimer = Millis + ScreenSaverTime;

    }
    else
    {
      ScreenSaverTimer = Millis + ScreenOnTime;
      ForceDraw = true;
    }

  }

  
  if (!ScreenSaverOn && (ForceDraw || Globals::MyPanel.NeedsDrawing()))
  {
    Globals::MyPanel.Draw(Globals::Oled);
    Globals::Oled.display();
  }
}
