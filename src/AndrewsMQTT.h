#ifndef _AndrewsMQTT_h
#define _AndrewsMQTT_h

#include <vector>
#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>

#define MaxClientIDLength 25			// Max Length for a Client ID
#define MaxTopicNameLength 50
#define MQTTConnectionTime 60000        // Milliseconds between MQTT Connection checks

//#include <Ethernet.h>

class MQTTMessage
{
	char Topic[MaxTopicNameLength];
	byte* payload;

	MQTTMessage(int PayloadSize) {      };


};


class AndrewsMQTT
{

	public:
		AndrewsMQTT(){	SetDefaultClientID(); };

		String ClientID = "NotSet";
		WiFiClient MyWifiClient;
		std::vector<String *> Topics;


		//IPAddress MQTT_Server = IPAddress(192, 168, 2, 208);
		
		
		char *MQTT_Server = "192.168.2.208";
		int MQTT_Port = 1883;
		int TopicRetentionLength=50;	// How man topics to remember for when re-subscribing after an mqtt connection losss/regain
		int HeartbeatTime = 30000;			// How long between heartbeats
		String HeartbeatTopic = "Sensors/Heartbeats";
		
		PubSubClient MyMQTTClient = PubSubClient(MyWifiClient);

		void setup();					// Call this from your Setup function
		void loop();					// Call this from your own loop() function
		
	
		// Functions
		void SetDefaultClientID();
		void SendHeartbeat();
		bool CheckMQTTConnection(long MaxWaitTime = 3000);
		int IndexOfSubscription(String *TopicName);
		void SubscribeToMQTTTopic(char *TopicName);
		void ReSubscribeToMQTTTopics();
		//void Publish(char *Message) {  MyMQTTClient->publish(Message);  };
		//static void MQTTCallback(char* topic, byte* payload, unsigned int length);

		private:
			bool AllSetUp = false;				// When Setup has been called, this is set to true
			WiFiClient espClient;

};

#endif