#include "AndrewsMQTT.h"
#include "Timer.h"


/*

Use this code in the MQTTCallback for your own callback function

MyMQTT.MyMQTTClient.setCallback(MQTTCallback);

void MQTTCallback(char* topic, byte* payload, unsigned int length)
{
 
	Serial.println(topic);
	Serial.print("Message:");

	for (int i = 0; i < length; i++) {
		Serial.print((char)payload[i]);
	}

	Serial.println("");


}


*/

// Sets the client ID to the default value of IPAddress
void AndrewsMQTT::SetDefaultClientID()
{
	ClientID = "ESP_";
	ClientID += WiFi.macAddress();

	//Serial.println(ClientID);


	int Index=0;

	while(1){		// Remove all the colans ":"
		Index = ClientID.indexOf(":");
		if(Index >= 0)
			ClientID.remove(Index,1);
		else
			break;
	};
}


void AndrewsMQTT::SendHeartbeat()
{
	String S = "{";
	
	S += "\"ClientID\": \"" + ClientID + "\","
	+ "\"Millis\": \"" + millis() + "\","
	+ "\"IP\": \"" + MyWifiClient.localIP().toString() + "\""
	+ "}";

	MyMQTTClient.publish(HeartbeatTopic.c_str(), S.c_str());

	Serial.printf("\nSending Heartbeat: %s\n", S.c_str());
/*
	Serial.print(" Topic: ");
	Serial.println(HeartbeatTopic.c_str());
	Serial.print(" MQTT Status = ");
	Serial.println(MyMQTTClient.state());
*/

}



void AndrewsMQTT::setup()
{

	PubSubClient client(WiFiClient); //espClient);	

	//Serial.printf("AAA\n");

	//Serial.printf("Server=%s Port=%d\n",MQTT_Server, MQTT_Port);
	MyMQTTClient.setServer(MQTT_Server, MQTT_Port);
	//MyMQTTClient.setCallback(MQTTCallback);
	//Serial.printf("BBB\n");
	//Serial.printf(" ClientID=\"%s\"\n", ClientID.c_str());

	//MyWifiClient.connect(ClientID.c_str());
	//MyMQTTClient.connect("UniqueID");

	AllSetUp = CheckMQTTConnection(6000);

	//Serial.printf("CCC");


	//AllSetUp = true;

	//return AllSetUp;
}

void AndrewsMQTT::loop()
{
    static long TimerHeartBeat = 0;
    static long MQTTConnectionCheck = 0;
	long M = millis();
    //static Timer TimerHeartbeat = Timer(HeartbeatTime);
	//static Timer MQTTConnectionCheck = Timer(250);

	if(!AllSetUp) setup();				// Eliminates the need for an explicit call to Setup()

	// Check MQTT Connection b4 touching any MQTT stuff
	if(M > MQTTConnectionCheck)
	{
        MQTTConnectionCheck = millis() + MQTTConnectionTime;
		CheckMQTTConnection(3000);

	}

	if(millis() > TimerHeartBeat)
    {				// If timer is expired, retrigger and send heartbeat
        TimerHeartBeat = millis() + HeartbeatTime;
    	SendHeartbeat();
    
    	//Serial.printf("RetriggerTime=%lu",TimerHeartbeat.ReTriggerTime);
	}
	
	MyMQTTClient.loop();
}



// below is causing the device to reset... not sure why, maybe polling speed? dunno

// Returns true for success, false for fail (after a maximum wait time of MaxWaitTime)
bool AndrewsMQTT::CheckMQTTConnection(long MaxWaitTime)
{

	static unsigned long PingTime = 0;
	bool ReturnValue = false;

//	Serial.printf(" (Check MQTT Conn: Address=%lu) ", &Topics);


	//Serial.printf("Checking MQTT collection, state=%d\n",MyMQTTClient.state());


	if (MyMQTTClient.connected() == 1)     // Are we connected?
		return true;


	//Serial.printf("\nMQTT Connection Status=%d State=%d WifiConnected=%d\n", MyMQTTClient.connected(), MyMQTTClient.state(),WiFi.isConnected());

	//Serial.printf("\nConnecting to MQTT Server %s\n",MQTT_Server);

	//Timer T(MaxWaitTime);
	long T = millis() + MaxWaitTime;
    while(1){
		Serial.print(".");
		if(millis() > T){
			Serial.printf("\n");
			return false;
		}
		//Serial.print("o");

		bool Success = MyMQTTClient.connect(ClientID.c_str());

		//Serial.print("SS");
		if (Success)
		{

			Serial.printf("\nConnected\nResubscribing to topics\n");
//				Serial.printf("\nCheckMQTTConnection: Address=%lu", &Topics);
			delay(200);	
			ReSubscribeToMQTTTopics();
			//Serial.print("TT");
		    Serial.print("Recovered MQTT Connection");
			return true;
		}
	    Serial.print("LOST MQTT Connection");
	}
}

// Returns the location within the Topics array of the given String/Topic, if it exists. -1 otherwise.
int AndrewsMQTT::IndexOfSubscription(String *TopicName)
{
	for(int c=0; c<Topics.size();  c++){
		if(*Topics[c] == *TopicName)
			return c;
	}

	return -1;  // (int)IndexOfSubscription(TopicName);
}

void AndrewsMQTT::SubscribeToMQTTTopic(char *TopicName)
{
	String *S = new String(TopicName);

	if (IndexOfSubscription(S) >= 0)
		return;

	Topics.push_back(S);

//	Serial.printf("Subsribing to %s IndexSize=%d\n",TopicName,Topics.size());
	MyMQTTClient.subscribe(TopicName);
	//Serial.println("");
}

// Subscribes to each topic in the Topics array. USeful for remaking a wifi connection without rebooting
void AndrewsMQTT::ReSubscribeToMQTTTopics()
{
	int Length = Topics.size();
	String *S;

	//Serial.printf("\nRe-adding %d MQTT Subscriptions...\n",Length);
//	Serial.printf("\nResubscribe: Address=%lu", &Topics);
	//char _TopicName[mqtt_MaxTopicNameLength];

	for (int c = 0; c < Length; c++)
	{
	//	Topics[c].toCharArray(_TopicName, mqtt_MaxTopicNameLength);

		//MQTT_Client.subscribe(_TopicName);
		//Serial.printf("AAA");
		S=Topics[c];
		//Serial.printf("BBB");
		MyMQTTClient.subscribe(S->c_str());
		//Serial.printf("CCC");

		//Serial.printf("re-Adding Subscription on Topic %s\n", S->c_str());
//		Serial.printf("re-Adding Subscription on Topic %s\n", *Topics[c]->c_str());
	}

}
