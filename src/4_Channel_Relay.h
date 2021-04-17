#ifndef _4_Channel_Relay_H
#define _4_Channel_Relay_H


#include <Arduino.h>
using namespace std;
#include <iostream>


# define nChannels 4


class Four_Channel_Relay{
    public:
        int ChannelGPIOPins[nChannels]={16,17,18,19};
        bool InvertStates = true;                       // A logic zero sent to GPIO pins results in an ON condition instead of OFF, ie: Current sink


        // Call this before using the relay
        // Sets the PinModes to OUTPUT and allows writing to the pins setup in ChannelGPIOPins.
        void Begin()
        {
            for(int i = 0; i<nChannels; i++){
                pinMode(ChannelGPIOPins[i], OUTPUT);
            }

        }

        // Returns the GPIO pin assigned to the channel.  Does safety/bounds checking
        int ChannelGpioPin(int Channel)
        {
            if(Channel < 0) Channel = 0;
            if(Channel > nChannels - 1) Channel = nChannels -1;

            return ChannelGPIOPins[Channel];
        }


        // Returns true if the relay is ON, false otherwise.  Takes into consideration InverStates
        bool ChannelState(int ChannelNum)
        {
            int State = digitalRead(ChannelGpioPin(ChannelNum));
            return (InvertStates) ? 1-State : State;

        }

        // Turn On or Off the Relay.  use true for ON and false for OFF
        void SetChannel(int ChannelNum, bool OnOff)
        {
            int PinNum = ChannelGpioPin(ChannelNum);
            digitalWrite(PinNum, (InvertStates) ? !OnOff : OnOff    );

            //cout << "Writing " << ((InvertStates) ? !OnOff : OnOff) << " to " << PinNum << " Channel=" << ChannelNum << endl;

        }

        void SetAllChannels(bool OnOff)
        {
            for(int i = 0; i<nChannels; i++){
                SetChannel(i,OnOff);
            }
        }

};



#endif
