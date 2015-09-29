#ifndef __SCREEN_h
#define __SCREEN_h

#include <Adafruit_GFX.h>   //Core graphics library
#include <Adafruit_ST7735.h> //hardware-specific library
#include <SPI.h> //??

// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     10
#define TFT_RST    0  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     8

//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

#define COLOR_BG ST7735_WHITE

class Screen
{
  public:
    static Screen& Instance()
    {
      static Screen instance;
      return instance;
    }
    
    void Init();
    void Draw(short xpos, short ypos, short width, short height, int* pixels);
    void DrawPixel(short xpos, short ypos, int pixel);
    void DrawRect(short xpos, short ypos, short width, short height, int color);
    int Width();
    int Height();
    
    //going to need something here to keep track of the BG tiles

    Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);;
  private:
    Screen();
    Screen(Screen const&);
    void operator=(Screen const&);

    
    
};
#endif
