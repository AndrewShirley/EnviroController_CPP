#ifndef _Image_H
#define _Image_H
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <iostream>


class PanelImage:public GUIControl
{
    private:
        void _setOffsetXY(int X, int Y)
        {
            int OldX = this->OffsetX;
            int OldY = this->OffsetY;
            this->OffsetX = X;
            this->OffsetY = Y;
            this->EnsureBounds();
            this->NeedsDrawing = (OldX != this->OffsetX) || (OldY != this->OffsetY);

            if (this->NeedsDrawing)
            {
               //Serial.printf("Needs Drawing in Image.h\n");
            }
        }

    public:
        const unsigned char *Image = nullptr;
        int OffsetX = 0, OffsetY = 0;                   // Offset into the image to display,(0,0)=Left,Top

        int getOffsetX() { return this->OffsetX; };
        int getOffsetY() { return this->OffsetY; };
        void setOffsetX(int X) {  _setOffsetXY(  X, this->OffsetY);  }
        void setOffsetY(int Y) {  _setOffsetXY(  this->OffsetX, Y);   };
        void addOffsetX(int X) {  _setOffsetXY( this->OffsetX + X, this->OffsetY);   };
        void addOffsetY(int Y) {  _setOffsetXY( this->OffsetX, this-> OffsetY + Y); };
        void addOffset(int X, int Y) {  _setOffsetXY(  this->OffsetX + X, this->OffsetY + Y);    };

        // Ensures the boundaries of OffsetX and OffsetY aren't out of the image
        void EnsureBounds()
        {
            int MaxRows = DontPanicSize / 16;               // Calculate number of rows available in the image
            int MaxRow = (MaxRows - 1) - this->Height;      // Find the maximum Row Number we can set OffsetY to in order NOT to overrun buffer
            if(this->OffsetX < 0) this->OffsetX = 0;
            if(this->OffsetY < 0) this->OffsetY = 0;
            if(this->OffsetX >= this->Width) this->OffsetX = this->Width - 1;
            if(this->OffsetY > MaxRow) this->OffsetY = MaxRow;
        }



        PanelImage(int X,int Y,int Width,int Height, const unsigned char *Image) : GUIControl(X,Y,Width,Height)
        {
            this->Image = Image;
            this->BorderType = BorderTypes::None;
        }


        void DrawContent(Adafruit_GFX &Gfx)
        {
            int Offset = this->OffsetY * 16 + (this->OffsetX / 8);

            //Offset = 16 * 40;

            Gfx.drawBitmap(this->X, this->Y, this->Image + Offset, this->Width, this->Height, WHITE, BLACK);
            this->NeedsDrawing = false;
        }

};


#endif

