#ifndef _Wifi_H
#define _Wifi_H

#include <WiFi.h>
#include <Arduino.h>
#include <ESPmDNS.h>


class WifiConnector
{
    public:
        char *SSID;
        char *Password;
        char *HostName = "enviro";


    // Is the Wifi Connected?
    bool IsConnected()
    {
        return WiFi.status() == WL_CONNECTED;
    }

    // Attempts to connect to the Wifi network.  Returns connection status, true=connected, false=not so much
    bool Connect(unsigned long MaxWaitTimeMilliseconds=5000)
    {
        unsigned long StartTime = millis();
      
     
        if(WiFi.status() != WL_CONNECTED) 
        {
            Serial.printf("\nBeginning WIFI Connection to %s\n",this->SSID);
            WiFi.begin(this->SSID, this->Password);
        }

        while(WiFi.status() != WL_CONNECTED     &&   (millis() - StartTime) + 200  < MaxWaitTimeMilliseconds) 
        {
            Serial.print(".");
            delay(200);
        }    

        if(WiFi.status() == WL_CONNECTED)
        {
            if (MDNS.begin(this->HostName))
                Serial.println("MDNS responder started");
        }


        return WiFi.status();
    }

    WifiConnector(){}
    WifiConnector(char *SSID, char *Password)
    {
        this->SSID = SSID;
        this->Password = Password;
    }

};



#endif