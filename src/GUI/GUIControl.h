#ifndef _GUIControl_H
#define _GUIControl_H

#include <Adafruit_GFX.h>
#include "GUI/GraphicObjectBase.h"
#include <map>

using namespace std;
class GUIControl;

struct KeyPress
{
    int Direction = 0;
    GUIControl *Goto = nullptr;
};

class GUIControl:public GraphicObjectBase
{
    private:
        std::map<string, KeyPress *> KeyPresses = {};

    public:
        KeyPress *AddKeypress(string KeyName, int Direction, GUIControl *Goto){
            KeyPress *K = new KeyPress();
            K->Direction = Direction;
            K->Goto = Goto;
            KeyPresses[KeyName] = K;
        }


        KeyPress *AddKeypress(int Direction, GUIControl *Goto){
            string KeyName = "kp_" + millis();
            return AddKeypress(KeyName,Direction,Goto);
        }


        // Returns the first KeyPress object that matches Direction. Returns nullptr if no match is found
        KeyPress *Matches(int Direction)
        {
            for(auto const& I : this->KeyPresses)
                if(I.second->Direction == Direction)
                    return I.second;
            
            return nullptr;        
        }



        GUIControl(int X, int Y, int Width, int Height) : GraphicObjectBase(X,Y,Width,Height)
        {

        };

    

        // Return true to continue handling Input, false to stop
        bool HandleInput() { return false; };



};






#endif

