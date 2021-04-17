#include <Arduino.h>
#include <GUI/GraphicObjectBase.h>
#include <GUI/GUI_Text.h>
#include <Adafruit_GFX.h>
using namespace std;
#include <iostream>
#include <pgmspace.h>
#include <string>
//#define pgm_read_byte(addr) (*(const unsigned char *)(addr))


void GUI_Text::CalculateTextAlignment()
{
    int TextWidth=this->TextWidth(), TextHeight=this->GetFontHeight();

    switch(this->HorizontalAlignment){
        case Centre:
            this->TextX = this->getWidth() / 2 - TextWidth / 2;
            //Serial.printf("Setting TextX to %d getWidth()=%d",TextX, this->getWidth());
            break;
        case Right:
            this->TextX = this->getWidth() - TextWidth;
            break;
    }

    switch(this->VerticalAlignment){
        case Middle:
            this->TextY = this->getHeight() / 2 - TextHeight / 2;
            break;
        case Bottom:
            this->TextY = this->getHeight();
            break;
    }
}




// Untested
// void GUI_Text::CalculateTextAlignment_(Adafruit_GFX &Gfx)
// {
//     uint16_t TextWidth=8, TextHeight=8;
//     int16_t x=0, y=0;

//     if(this->Font)
//     {
//         Gfx.getTextBounds(this->Text, 0,0,&x,&y,&TextWidth,&TextHeight);
        
//         switch(this->HorizontalAlignment){
//             case Centre:
//                 this->TextX = this->getWidth() / 2 - TextWidth / 2;
//                 break;
//             case Right:
//                 this->TextX = this->getWidth() - TextWidth;
//                 break;
//         }

//         switch(this->VerticalAlignment){
//             case Middle:
//                 break;
//             case Bottom:
//                 break;
//         }
//     }
// }


int GUI_Text::GetFontHeight()
{
    if(this->Font)
    {
        uint16_t first = this->Font->first;
        GFXglyph *glyph = this->Font->glyph + uint16_t('A') - first;
        return (int)glyph->height;
    }

    return 8;
}


void GUI_Text::DrawContent(Adafruit_GFX &Gfx)
{
    GFXcanvas1 C(this -> getWidth(), this -> getHeight());
    C.setTextWrap(this -> TextWrap);

    if(this->Font)
    {
        int Y = this->getTextY();
        int Height = this->GetFontHeight();
        int TextBase = Y + Height;

        //cout << "TB=" << TextBase << " Y=" << Y << " Height=" << Height << "    ";
        C.setFont(this->Font);
        C.setCursor(this->TextX - this->HorizontalScrollPosition, TextBase);
    }
    else
        C.setCursor(this->TextX - this->HorizontalScrollPosition, this->TextY);//this->getHeight() - this->TextY);

    C.write(Text.c_str());
    Gfx.drawBitmap(this -> getX(),this -> getY(),C.getBuffer(),this->getWidth(),this->getHeight(),this->getTextColour(),0);
    
    
    //Serial.print("DraweConent on Text called");
    //string temp;
    //cout << "Press a key to continue";
    //cin >> temp;
}


// Returns the cached version of TextWidth. Its automatically recalculated each time Text is changed
int GUI_Text::TextWidth()
{
    return(this->_TextWidth);
}

int GUI_Text::CalculateTextWidth()
{
    int Width = 0;

    if (Font)
    {
        uint8_t First = pgm_read_byte(&Font->first);
//      uint8_t Last = pgm_read_byte(&Font->last);
        GFXglyph *glyph;

        for(char ch : this -> Text)
        {
            glyph = Font->glyph + ch - First;
        
            Width += glyph->xAdvance;
            //cout << "Glyph Ch=" << ch << " width=" << (int)glyph->width;
        }
    }
    else
    {
        int Len = Text.length();
        if(Len > 1)
            Width = Len * 6 + (Len - 1) * 6;                // Characters are 6 pixels wide, add spaces
        else
            Width = 6 * Len;

    }
    //cout << "Text:" << Text << " Width:" << Width << endl;
    this->_TextWidth = Width;
    return Width;
}


GUI_Text::GUI_Text(int X, int Y, int Width, int Height, String Text) : GUIControl(X,Y,Width,Height)
{
    //GraphicObjectBase:GraphicObjectBase(X,Y,Width,Height);
    this -> Text = Text;
    this -> CalculateTextWidth();
    this -> setNeedsDrawing(true);

}

