/*



*/

#include <iostream>
#include <GUI/GraphicObjectBase.h>
#include <Arduino.h>
#include <Adafruit_GFX.h>

using namespace std;

GraphicObjectBase::GraphicObjectBase(int X, int Y, int Width, int Height)
{
    static int n = 0;

    snprintf(this->UniqueID, MaxUniqueIDLength, "GO_%ul", millis());
    this->X = X;
    this->Y = Y;
    this->Width = Width;
    this->Height = Height;
    this->NeedsDrawing = true;
}



// Meant to be overridden
void GraphicObjectBase::DrawContent(Adafruit_GFX &Gfx)
{
    Serial.print("CXalled GraphicBaseObjectr DrawContent function");
}

void GraphicObjectBase::DrawBackground(Adafruit_GFX &Gfx)
{
    switch (BorderType)
    {
    case Square:
        if (BackgroundColour > -1)
            Gfx.fillRect(X, Y, Width, Height, BackgroundColour);
        break;
    case Rounded:
        if (BackgroundColour > -1)
            Gfx.fillRoundRect(X, Y, Width, Height, BorderCornerRadius, BackgroundColour);
        break;
    }
}

void GraphicObjectBase::DrawSelectedMark(Adafruit_GFX &Gfx)
{
    switch (BorderType)
    {
    case Square:
        Gfx.drawLine(X + 1, Y, X + 1, Y + Height, BorderColour);
        break;
    case Rounded:
        int Y1 = Y + BorderCornerRadius;
        int Y2 = Y + Height - BorderCornerRadius;

        if (Y1 < Y2)
            Gfx.drawLine(X + 1, Y1, X + 1, Y2, BorderColour);
        Gfx.drawLine(X + 2, Y1 - 1, X + 2, Y2 + 1, BorderColour);
        break;
    }
}

void GraphicObjectBase::DrawBorder(Adafruit_GFX &Gfx)
{
    switch (BorderType)
    {
    case Square:
        Gfx.drawRect(X, Y, Width, Height, BorderColour);
        break;
    case Rounded:
        Gfx.drawRoundRect(X, Y, Width, Height, BorderCornerRadius, BorderColour);
        break;
    }
}




void GraphicObjectBase::DrawDimmer(Adafruit_GFX &Gfx)
{
    int Counter = 0; // Playing with MaxCount can result in neato patterns, especially odd ones. Some Even ones may not work as expected due to repetition in the pattern
    //cout << "Dimming" << this->UniqueID <<  endl;


    for (int x = 0; x < this->Width; x++)
        for (int y = 0; y < this->Height; y++)
            if (Counter++ > this->DimmerPattern)
            {
                Counter = 0;
                Gfx.drawPixel(x + this->X, y + this->Y, 0);
                //cout << x << "," << y << endl;
            }
}


void GraphicObjectBase::Draw(Adafruit_GFX &Gfx)
{
    //cout << "Draw " << (char *)this->UniqueID << endl;
    DrawBackground(Gfx);
    DrawContent(Gfx);
    DrawBorder(Gfx);
    if (Selected)
        DrawSelectedMark(Gfx);
    if (Dim)
        DrawDimmer(Gfx);
    this->NeedsDrawing = false;
}
