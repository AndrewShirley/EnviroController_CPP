#ifndef _GraphicObjectBase_H
#define _GraphicObjectBase_H
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <iostream>

#define MaxUniqueIDLength 20

class GraphicObjectBase
{
    public:
        enum BorderTypes   {None, Square, Rounded};
    
    protected:
        bool NeedsDrawing = false; // Can set this to True to force a redaw of the component. See NeedsDrawing() - Can be overridden for 
        int X = 0, Y = 0;
        int Width = 128;
        int Height = 64;
        int BackgroundColour = 0; // -1 = Transparent, only write the 1's
        int BorderColour = 1;
        int BorderCornerRadius = 4;
        bool Selected = false;
        BorderTypes BorderType = Square;
        bool Visible = true;
        char UniqueID[MaxUniqueIDLength];
        bool Dim = false;
        int DimmerPattern = 2;

    public:
        bool getNeedsDrawing(){ return this -> NeedsDrawing; }
        void setNeedsDrawing(bool NewValue){ this -> NeedsDrawing = NewValue; }

        bool getDim(){ return this -> Dim; }
        void setDim(bool NewValue){ if(NewValue != this->Dim) { std::cout << "Dim=" << NewValue << " " << this->UniqueID << std::endl; this-> Dim = NewValue; this->NeedsDrawing = true; }}

        void setVisible(bool isVisible) { if(this->Visible != isVisible){ this->Visible = isVisible; this->NeedsDrawing = true; }}
        bool getVisible() { return this->Visible; }

        BorderTypes getBorderType(){ return this -> BorderType; }
        void setBorderType(BorderTypes NewValue){ if(NewValue != this -> BorderType){ this -> BorderType = NewValue; this -> NeedsDrawing = true; }}
        void setXY(int NewX, int NewY){ if(NewX != this->X || NewY != this->Y){ this ->X = NewX; this->Y = NewY ;this -> NeedsDrawing = true; }}

        char *GetUniqueID(){ return this->UniqueID; };
        
//        void SetUniqueID(String *UniqueID) {  delete this->UniqueID;  this->UniqueID = new String(*UniqueID); };
        void SetUniqueID(char *UniqueID) { strncpy(this->UniqueID, UniqueID, MaxUniqueIDLength); this->UniqueID[MaxUniqueIDLength-1] = 0; };

        int getX(){ return this -> X; }
        void setX(int NewValue){ if(NewValue != this -> X){ this -> X = NewValue; this -> NeedsDrawing = true; }}

        int getY(){ return this -> Y; }
        void setY(int NewValue){ if(NewValue != this -> Y){ this -> Y = NewValue; this -> NeedsDrawing = true; }}

        int getWidth() { return this -> Width; }
        void setWidth(int NewValue){ if(NewValue != this -> Width){ this -> Width = NewValue; this -> NeedsDrawing = true; }}

        int getHeight() { return this -> Height; }
        void setHeight(int NewValue){ if(NewValue != this -> Height){ this -> Height = NewValue; this -> NeedsDrawing = true; }}

        int getBackgroundColour() { return this -> BackgroundColour; }
        void setBackgroundColour(int NewValue){ if(NewValue != this -> BackgroundColour){ this -> BackgroundColour = NewValue; this -> NeedsDrawing = true; }}

        int getBorderColour() { return this -> BorderColour; }
        void setBorderColour(int NewValue){ if(NewValue != this -> BorderColour){ this -> BorderColour = NewValue; this -> NeedsDrawing = true; }}

        int getBorderCornerRadius() { return this -> BorderCornerRadius; }
        void setBorderCornerRadius(int NewValue){ if(NewValue != this -> BorderCornerRadius){ this -> BorderCornerRadius = NewValue; this -> NeedsDrawing = true; }}

        bool getSelected() { return this -> Selected; }
        void setSelected(bool NewValue){ if(NewValue != this -> Selected){ this -> Selected = NewValue; this -> NeedsDrawing = true; }}

        void Draw(Adafruit_GFX &Gfx);
        void DrawBackground(Adafruit_GFX &Gfx);
        virtual void DrawContent(Adafruit_GFX &Gfx);
        void DrawBorder(Adafruit_GFX &Gfx);
        void DrawSelectedMark(Adafruit_GFX &Gfx);           // If this component is selected, draws a Mark to show it
        void DrawDimmer(Adafruit_GFX &Gfx);                 // If this component is Dimmed, Dim it
        
        GraphicObjectBase(int X, int Y, int Width, int Height);
};

#endif // Closes _GraphicObjectBase_H
