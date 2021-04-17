#ifndef _GUI_Text_H
#define _GUI_Text_H

#include <Adafruit_GFX.h>
#include "GUI/GraphicObjectBase.h"
#include "GUI/GUIControl.h"
using namespace std;


class GUI_Text:public GUIControl
{
    private:
        int TextColour = 1;
        int TextX = 2;              // Where within the frame to place the text, Y is vertical, +ve = up
        int TextY = 1;
        bool TextWrap = false;
        String Text = "Hello World";
        int _TextWidth = 0;         // Stores the last calculated width and returns that instead of recalculating.


    public:
        int CalculateTextWidth();
        int TextWidth();
        int GetFontHeight();
        enum VAlignment {Top, Middle, Bottom};
        enum HAlignment {Left, Centre, Right};

        GUI_Text::VAlignment VerticalAlignment = Top;
        GUI_Text::HAlignment HorizontalAlignment = Left;

        const GFXfont *Font = nullptr;
        void setFont(const GFXfont* Font){ this->Font = Font; }
        
        GUI_Text(int X, int Y, int Width, int Height, String Text);
        void DrawContent(Adafruit_GFX &Gfx);
        void CalculateTextAlignment();

        String getText() { return this -> Text; }
        void setText(char *Buf){
            this->setText(String(Buf));
        }

        void setText(String NewValue){
            if(NewValue != this -> Text)
            {
                this -> Text = NewValue;
                this->setNeedsDrawing(true);
                this->CalculateTextWidth();
                this->CalculateTextAlignment();
            }
        }

        bool getTextWrap() { return this -> TextWrap; }
        void setTextWrap(bool NewValue){ if(NewValue != this -> TextWrap){ this -> TextWrap = NewValue; this->setNeedsDrawing(true); }}

        int getTextColour() { return this -> TextColour; }
        void setTextColour(int NewValue){ if(NewValue != this -> TextColour){ this -> TextColour = NewValue; this->setNeedsDrawing(true); }}

        int getTextX() { return this -> TextX; }
        void setTextX(int NewValue){ if(NewValue != this -> TextX){ this -> TextX = NewValue; this->setNeedsDrawing(true); }}

        int getTextY() { return this -> TextY; }
        void setTextY(int NewValue){ if(NewValue != this -> TextY){ this -> TextY = NewValue; this->setNeedsDrawing(true); }}

        int GetHorizontalScrollPosition(){ return this -> HorizontalScrollPosition; }
        void SetHorizontalScrollPosition(int NewPosition)
        {
            if(this->HorizontalScrollPosition != NewPosition)
            {
                this -> HorizontalScrollPosition = NewPosition;
                this -> setNeedsDrawing(true);
            }
        }
    protected:
        int HorizontalScrollPosition = 0;     // How many Pixels to shift the text by. +/- = right/left
};

#endif

