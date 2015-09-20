#include <Adafruit_GFX.h>   //Core graphics library
#include <Adafruit_ST7735.h> //hardware-specific library
#include <avr/pgmspace.h>
#include <SPI.h> //??
#include "SpriteDefinitions.h"

// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     10
#define TFT_RST    0  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     8

// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);


int playerX;
int playerY;
uint16_t playerWidth;
uint16_t playerHeight;
int lastPlayerX = -1;
int lastPlayerY = -1;
int playerSpeed = 10;
uint16_t bgcolor = ST7735_BLACK;

//clock stuff
long startTime;
int count = 0;
int lastFPS = 0;

void setup() {  
  //setup serial communication
  Serial.begin(9600);
  
  // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.setRotation(1);
  tft.fillScreen(bgcolor);

  playerWidth = (uint16_t)pgm_read_word_near(PC_WIDTH);
  playerHeight = (uint16_t)pgm_read_word_near(PC_HEIGHT);
  
  playerX = (tft.width() / 2) - (playerWidth / 2);
  playerY = (tft.height() / 2) - (playerHeight / 2);

  startTime = millis();
}

void clearSprite()
{
  tft.fillRect(lastPlayerX, lastPlayerY, playerWidth, playerHeight, bgcolor);
}

void drawSprite()
{
  for ( int x = 0; x < playerWidth; x++)
  {
    for (int y = 0; y < playerHeight; y++)
    {
      int index = (y * playerWidth) + x;

      int value = pgm_read_word_near(PC + index);
      tft.drawPixel(playerX + x, playerY + y, value);
    }
  }
}

void movePlayer()
{
  float UpDown = ((analogRead(A7) / 1020.0) - 0.5) * -1;
  float LeftRight = ((analogRead(A6) / 1020.0) - 0.5 ) * -1;

  lastPlayerX = playerX;
  playerX = playerX + (int)(playerSpeed * LeftRight);
  if (playerX + playerWidth > tft.width()) playerX = tft.width() - playerWidth;
  if (playerX < 0) playerX = 0;

  lastPlayerY = playerY;
  playerY = playerY + (int)(playerSpeed * UpDown);
  if (playerY + playerHeight > tft.height()) playerY = tft.height() - playerHeight;
  if (playerY < 0) playerY = 0;

//  Serial.print("X: ");
//  Serial.print(playerX, DEC);
//  Serial.print("  Y: ");
//  Serial.println(playerY, DEC); 
  
  if (lastPlayerX != playerX || lastPlayerY != playerY)
  {
    clearSprite();
    drawSprite();    
  }
}

void printFPS(int fps, uint64_t color)
{
  tft.setCursor(1,1);
  tft.setTextColor(color);
  tft.setTextSize(1);
  tft.print(fps, DEC);
}

void loop() {  
  count++;
  if (millis() - startTime >= 1000)
  {
    printFPS(lastFPS, bgcolor);
    lastFPS = count;
    printFPS(lastFPS, ST7735_WHITE);
    
    startTime = millis();
    count = 0;
  }
  
  
  movePlayer();   
   //delay(10);  
}
