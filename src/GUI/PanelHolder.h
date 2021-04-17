#ifndef _PanelHolder_H
#define _PanelHolder_H

#include <map>
#include <iostream>
#include "GUI/GraphicObjectBase.h"
#include "GUI/GUIControl.h"
using namespace std;

// // Extends GraphicObjectBase to add GUI Framework
// class PanelGraphicObject : GraphicObjectBase{
//     GUIControl *GraphicObject;
//     // The following are used by the PanelHolder to determine where to move next when user input comes in
//     bool Selectable = true;
//     GraphicObjectHolder *Move_Right = nullptr;
//     GraphicObjectHolder *Move_Up = nullptr;
//     GraphicObjectHolder *Move_Left = nullptr;
//     GraphicObjectHolder *Move_Down = nullptr;
// };

class PanelHolder
{
private:
    //std::map<string, GUIControl *> Items = {};
    std::map<string, GUIControl *> Items = {};

public:
    bool Visible = true;
    GUIControl *InputTarget = nullptr; // Which GUI Control should input be sent to? nullptr = this panel will handle it

    // Handles input from the user, ie: Rotary switch
    // If there is an InputTarget set, then that is given the Input.  The InputTarget returns true to continue handling input or false to stop handling input
    // Returns true if the Input was handled, false otherwise.
    // Direction:
    //      RIGHT: 316 to 0 Degrees and 0 to 45 Degrees
    //         UP:  46 to 135 Degrees
    //       LEFT: 136 to 225
    //       DOWN: 226 to 315
    // Magnitude is how far to travel along that path, ie: 1 item in the scroll list, 2 items , 3 items etc
    bool HandleInput(int Direction, int Magnitude, bool Click)
    {
        if (InputTarget != nullptr)
        {
            if (!(InputTarget->HandleInput())) // Calls the InputHandler. If it returns false, it should stop handling input
                InputTarget = nullptr;
            return true;
        }

        cout << "PanelHolder looking at input. Dir=" << Direction << endl;

        GUIControl *C = GetSelected();
        KeyPress *Next;
        if (C != nullptr)
        {

            cout << "A Old:'";
            cout << C->GetUniqueID();

            Next = C->Matches(Direction);
            if (Next != nullptr && Next->Goto != nullptr)
            {
                cout << "' B New:'";
                cout << Next->Goto->GetUniqueID();
                C->setSelected(false);
                Next->Goto->setSelected(true);
                cout << "' Needs Drawing = " << this->NeedsDrawing() << endl;
            }
        }

        cout << "Done" << endl;
    }

    // Returns the first selected Object in the Panel.  Returns nullptr if none found
    GUIControl *GetSelected()
    {
        for (auto const &I : this->Items)
            if (I.second->getSelected())
                return I.second;

        return nullptr;
    }

    // Finds the GUIControl containing the given GUIControl, otherwise returns nullptr
    GUIControl *GetItem(GUIControl *O)
    {
        for (auto const &I : this->Items)
            if (I.second == O)
                return I.second;

        return nullptr;
    }

    // void SetSelected(GUIControl *NewSelectedItem)
    // {
    //     GUIControl *G = GetItem(NewSelectedItem);

    //     if(G != nullptr)
    //         SetSelected(G);

    // }

    // DE-selects the currently selected item and selects the provided one
    // This is a good spot to issue GUIControl.Defocus and .Focus events, although for now it can be done
    // in the Base Object's SetSelected() function
    void SetSelected(GUIControl *NewSelectedItem)
    {
        GUIControl *OldSelected = this->GetSelected();

        if (OldSelected != nullptr)
        {
            OldSelected->setSelected(false);
        }
        if (NewSelectedItem != nullptr)
        {
            NewSelectedItem->setSelected(true);
        }
    }

    // Returns the number of visible items in the Panel
    unsigned int NumVisible()
    {
        int N = 0;
        for (auto const &I : this->Items)
            if (I.second->getVisible())
                N++;

        return N;
    }

    GUIControl *AddItem(string KeyName, GUIControl *H)
    {
        this->Items[KeyName] = H;
    }

    GUIControl *AddItem(string KeyName, GUIControl *O, GUIControl *Up, GUIControl *Left = nullptr, GUIControl *Down = nullptr, GUIControl *Right = nullptr)
    {
        if (Up != nullptr)
            O->AddKeypress("Up", 0, Up);
        if (Left != nullptr)
            O->AddKeypress("Left", 90, Left);
        if (Down != nullptr)
            O->AddKeypress("Down", 180, Down);
        if (Right != nullptr)
            O->AddKeypress("Right", 270, Right);

        AddItem(KeyName, O);
    }

    bool HasItem(string KeyName)
    {
        std::map<string, GUIControl *>::iterator IR;
        IR = this->Items.find(KeyName);

        return (IR != this->Items.end());
    }

    GUIControl *GetItem(string KeyName)
    {
        std::map<string, GUIControl *>::iterator IR;
        IR = this->Items.find(KeyName);

        if (IR != this->Items.end())
            return IR->second;

        return nullptr;
    }

    std::map<string, GUIControl *> GetItemsObj()
    {
        return this->Items;
    }

    // Checks all the items in the Items array to see if any needs drawing.
    bool NeedsDrawing()
    {
        for (auto const &I : this->Items)
            if (I.second->getNeedsDrawing())
                return true;

        return false;
    }

    void Draw(Adafruit_GFX &Gfx)
    {
        GUIControl *B;
        string KeyName;

        if (!Visible)
            return;

        //GUIControl::Draw(Gfx);

        //for(std::map<string, GUIControl *> I : this->Items)
        for (auto const &I : this->Items)
        {
            //KeyName = I.first;
            //B = I.second;

            //Serial.printf("Looking to draw Panel Item...%s\n", KeyName);
            if (I.second->getVisible())
            {
                I.second->Draw(Gfx);
            }
        }
    }

    void PrintItem(GUIControl *Item)
    {
        Serial.printf("X,Y=%d,%d UN=%s\n", Item->getX(), Item->getY(), Item->GetUniqueID());
    }

    // Finds the GUIControl containing the given GUIControl, otherwise returns nullptr
    void PrintItems()
    {
        for (auto const &I : this->Items)
        {
            PrintItem(I.second);
        }
    }
};

#endif

/*

Current Status: debugging why UniqueID comes out as an address instead of a string.
Relevant source files are open.+


*/