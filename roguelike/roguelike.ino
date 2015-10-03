
#include <Arduino.h>
#include <avr/pgmspace.h> //abililty to store variables in program space
#include "PDQ_ST7735_config.h" //screen configuration
#include <PDQ_FastPin.h>
#include <PDQ_GFX.h>   //Core graphics library
#include <PDQ_ST7735.h> //hardware-specific library
#include <SPI.h> //communication for screen
#include "SpriteDefinitions.h"
#include "Screen.h"
#include "Actor.h"
#include "Player.h"
#include "Level.h"
#include "Room.h"
#include "MemoryFree.h"

PDQ_ST7735 _tft; //global access to the screen needed by the library

//clock stuff
long startTime;
unsigned long count = 0;
unsigned long lastFPS = 0;

long lastInputMeasure;

int lastHPdraw = -1;
int lastFreeMem = 0;

Player player;
Level* CurrentLevel;

void setup() {  
  //setup serial communication
  Serial.begin(9600);

  //init the screen
  Screen* screen = &Screen::Instance();
  screen->Init();
  
  int playerX = (screen->Width() / 2) - (player.GetWidth() / 2);
  int playerY = (screen->Height() / 2) - (player.GetHeight() / 2);
  player.SetPosition(playerX, playerY);

  //draw status frame
  screen->DrawRect(0,0,160,24,ST7735_BLACK); 
  lastInputMeasure = startTime = millis();
}


void loop() {  
  measureFPS();
  
  drawHearts();

  if (millis() - lastInputMeasure >= 1000/60)
  {
    measureFreeMem();
    movePlayer();
    lastInputMeasure = millis();
  }
   
}

void drawHearts()
{
  if (lastHPdraw == player.HP)
  {
    return;
  }
  lastHPdraw = player.HP;
  
  const int startXpos = 130;
  const int startYpos = 5;
  const int maxHeartsPerRow = 4;
  const short heartWidth = (short)pgm_read_word_near(HP);
  const short heartHeight = (short)pgm_read_word_near(HP + 1);

  //load the images  
  int* pixelsFullHeart = new int[heartWidth* heartHeight];
  for (int i = 0; i < heartWidth* heartHeight; i++)
  {
    pixelsFullHeart[i] = pgm_read_word_near(HP + 2 + i);  
  }

  int* pixelsEmptHeart = new int[heartWidth * heartHeight];
  for (int i = 0; i < heartWidth* heartHeight; i++)
  {
    pixelsEmptHeart[i] = pgm_read_word_near(HP_EMPTY + 2 + i);  
  }
  
  
  
  //go through all the hearts that we have
  for (int i = 0; i < player.MaxHP; i++)
  {
    int drawXPos = startXpos + ((heartWidth + 1)  * (i % maxHeartsPerRow));
    int drawYPos = startYpos + ((heartHeight + 3) * (i / maxHeartsPerRow));

    int* pixels = pixelsEmptHeart;

    if (player.HP > i)
    {
      pixels = pixelsFullHeart;
    }
    
    Screen::Instance().Draw(drawXPos, drawYPos, heartWidth, heartHeight, pixels);
  }
  
  
}

void movePlayer()
{
  float UpDown = ((analogRead(A7) / 1020.0) - 0.5) * -1;
  float LeftRight = ((analogRead(A6) / 1020.0) - 0.5 ) * -1;

  player.Move(LeftRight, UpDown);
}

void measureFreeMem()
{
  int freeMem = freeMemory();

  if (freeMem == lastFreeMem)
  {
    return;
  }

  printMem(lastFreeMem, ST7735_BLACK);
  printMem(freeMem, ST7735_WHITE);
  lastFreeMem = freeMem;
}

void printMem(int freeMem, uint64_t color)
{
  __SetCursor(1,9);
  __SetTextColor(color);
  _tft.print(freeMem, DEC);
}

void measureFPS()
{
  count++;
  if (millis() - startTime >= 1000)
  {
    printFPS(lastFPS, ST7735_BLACK);
    lastFPS = count;
    printFPS(lastFPS, ST7735_WHITE);
    Serial.print("Free memory: ");
    Serial.print(freeMemory());
    Serial.println(" Bytes");
    
    startTime = millis();
    count = 0;
  }
}

void printFPS(unsigned long fps, uint64_t color)
{
  __SetCursor(1,1);
  __SetTextColor(color);
  _tft.print(fps, DEC);
  //Serial.print("FPS: ");
  //Serial.println(fps);
}

////////////////////////////////////////////////////////////////////////////////////////
/// Screen wrappers

void __Begin()
{
  #if defined(ST7735_RST_PIN)        // reset like Adafruit does
  FastPin<ST7735_RST_PIN>::setOutput();
  FastPin<ST7735_RST_PIN>::hi();
  FastPin<ST7735_RST_PIN>::lo();
  delay(1);
  FastPin<ST7735_RST_PIN>::hi();
#endif

  _tft.begin();
}

void __DrawPixel(int16_t x, int16_t y, uint16_t color)
{
  _tft.drawPixel(x,y,color);
}

void __DrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  _tft.drawRect(x,y,w,h,color);
}

void __DrawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
  _tft.drawCircle(x0,y0,r,color);
}

void __DrawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
  _tft.drawCircleHelper(x0,y0,r,cornername,color);
}

void __FillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
  _tft.fillCircle(x0,y0,r,color);
}

void __FillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color)
{
  _tft.fillCircleHelper(x0,y0,r,cornername,delta,color);
}

void __DrawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  _tft.drawTriangle(x0,y0,x1,y1,x2,y2,color);
}

void __FillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
  _tft.fillTriangle(x0,y0,x1,y1,x2,y2,color);
}

void __DrawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color)
{
  _tft.drawRoundRect(x0,y0,w,h,radius,color);
}

void __FillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color)
{
  _tft.fillRoundRect(x0,y0,w,h,radius,color);
}

void __DrawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size)
{
  _tft.drawChar(x,y,c,color,bg,size);
}

void __SetCursor(int16_t x, int16_t y)
{
  _tft.setCursor(x,y);
}

void __SetTextColor(uint16_t c)
{
  _tft.setTextColor(c);
}

void __SetTextColor(uint16_t c, uint16_t bg)
{
  _tft.setTextColor(c,bg);
}

void __SetTextSize(uint8_t s)
{
  _tft.setTextSize(s);
}

void __SetTextWrap(boolean w)
{
  _tft.setTextWrap(w);
}

int16_t __ScreenWidth()
{ 
  return _tft.width(); 
}
  
int16_t __ScreenHeight()
{ 
  return _tft.height(); 
}

uint8_t __GetRotation()
{ 
  return _tft.getRotation();
}

void __DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
  _tft.drawLine(x0,y0,x1,y1,color);
}

void __DrawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  _tft.drawFastVLine(x,y,h,color);
}

void __DrawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  _tft.drawFastHLine(x,y,w,color);
}

void __FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  _tft.fillRect(x,y,w,h,color);
}

void __FillScreen(uint16_t color)
{
  _tft.fillScreen(color);
}

void __SetRotation(uint8_t r)
{
  _tft.setRotation(r);
}

void __InvertDisplay(boolean i)
{
  _tft.invertDisplay(i);
}


