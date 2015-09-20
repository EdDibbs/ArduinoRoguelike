#include "Screen.h"

Screen::Screen()
{

}

void Screen::Init()
{
  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.setRotation(1);
  tft.fillScreen(COLOR_BG);
}

int Screen::Width()
{
  return tft.width();
}

int Screen::Height()
{
  return tft.height();
}

void Screen::Draw(short xpos, short ypos, short width, short height, int* pixels)
{
  for ( int x = 0; x < width; x++)
  {
    for (int y = 0; y < height; y++)
    {
      int index = (y * width) + x;

      int value = pixels[index];

      //because we're using all possible values of a uint16, we need to specify a 
      //color that will denote transparency. In this case, it's the equivalent of
      //magenta. These values will be ignored (not drawn).
      if (value != 0xF81F)
      {
        tft.drawPixel(xpos + x, ypos + y, value);
      }      
    }
  }
}

void Screen::DrawRect(short xpos, short ypos, short width, short height, int color)
{
  tft.fillRect(xpos, ypos, width, height, color);
}

