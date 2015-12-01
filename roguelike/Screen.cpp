#include "Macros.h"
#include "Screen.h"
#include <Arduino.h>

extern void __Begin();
extern void __DrawPixel(int16_t x, int16_t y, uint16_t color);
extern void __DrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
extern void __DrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
extern void __DrawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
extern void __FillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
extern void __FillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
extern void __DrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
extern void __FillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
extern void __DrawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
extern void __FillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
extern void __DrawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);
extern void __SetCursor(int16_t x, int16_t y);
extern void __SetTextColor(uint16_t c);
extern void __SetTextColor(uint16_t c, uint16_t bg);
extern void __SetTextSize(uint8_t s);
extern void __SetTextWrap(boolean w);
extern uint16_t __Color565(int r, int g, int b);
extern int16_t __ScreenWidth();  
extern int16_t __ScreenHeight();
extern uint8_t __GetRotation();
extern void __DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
extern void __DrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
extern void __DrawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
extern void __FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
extern void __FillScreen(uint16_t color);
extern void __SetRotation(uint8_t r);
extern void __InvertDisplay(boolean i);

Screen::Screen()
{

}

void Screen::Init()
{
   //most config is done in PDQ_ST7735_config.h
  __Begin();
  __SetRotation(1);
  __FillScreen(0x00);
}

int Screen::Width()
{
  return __ScreenWidth();
}

int Screen::Height()
{
  return __ScreenHeight();
}

void Screen::DrawPixel(short xpos, short ypos, uint16_t pixel)
{
  __DrawPixel(xpos, ypos, pixel);
}

void Screen::Draw(short xpos, short ypos, short width, short height, uint16_t* pixels)
{
  Draw(xpos, ypos, width, height, pixels, 0, 0);
}

void Screen::Draw(short xpos, short ypos, short width, short height, uint16_t* pixels, byte flip, byte rot)
{
  for ( int x = 0; x < width; x++)
  {
    for (int y = 0; y < height; y++)
    {
      int newX = x,newY = y;

      if (flip == FlipBoth)
      {
        newX = width - x - 1;
        newY = height - y - 1;
      }
      else if (flip == FlipHori)
      {
        newX = width - x -1;        
      }
      else if (flip == FlipVert)
      {
        newY = height - y - 1;
      }

      //limit to 3 rotations
      rot = rot % 4;
      for (int rotIndex = 0; rotIndex < rot; rotIndex++)
      {
        int tempX = newX, tempY = newY;

        newX = height - tempY - 1;
        newY = tempX;
      }

      int index = (newY * width) + newX;
      int value = pixels[index];

      //because we're using all possible values of a uint16, we need to specify a 
      //color that will denote transparency. In this case, it's the equivalent of
      //magenta. These values will be ignored (not drawn).
      if (value != 0xF81F)
      {
        __DrawPixel(xpos + x, ypos + y, value);
      }  
    }
  }
}

uint16_t* Screen::FlipAndRotateImage(short width, short height, uint16_t* pixels, byte flip, byte rot)
{
  uint16_t* out = new uint16_t[width * height];
  
  for ( int x = 0; x < width; x++)
  {
    for (int y = 0; y < height; y++)
    {
      int newX = x,newY = y;

      if (flip == FlipBoth)
      {
        newX = width - x - 1;
        newY = height - y - 1;
      }
      else if (flip == FlipHori)
      {
        newX = width - x -1;        
      }
      else if (flip == FlipVert)
      {
        newY = height - y - 1;
      }

      //limit to 3 rotations
      rot = rot % 4;
      for (int rotIndex = 0; rotIndex < rot; rotIndex++)
      {
        int tempX = newX, tempY = newY;

        newX = height - tempY - 1;
        newY = tempX;
      }

      int index = (newY * width) + newX;
      int value = pixels[index];

      out[(y * width) + x] =  pixels[index];      
    }
  }

  return out;
}

uint16_t* Screen::FlipImage(short width, short height, uint16_t* pixels, byte flip)
{
  return FlipAndRotateImage(width, height, pixels, flip, 0);
}
uint16_t* Screen::RotateImage(short width, short height, uint16_t* pixels, byte rot)
{
  return FlipAndRotateImage(width, height, pixels, 0, rot);
}

uint16_t Screen::Color565(int r, int g, int b)
{
  return __Color565(r,g,b);
}
    
void Screen::DrawRect(short xpos, short ypos, short width, short height, int color)
{
  __FillRect(xpos, ypos, width, height, color);
}



