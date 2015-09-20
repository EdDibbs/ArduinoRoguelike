#include <avr/pgmspace.h>
#include <Adafruit_GFX.h>   //Core graphics library
#include <Adafruit_ST7735.h> //hardware-specific library
#include <SPI.h> //??
#include "SpriteDefinitions.h"
#include "Screen.h"
#include "Player.h"

//clock stuff
long startTime;
int count = 0;
int lastFPS = 0;

Player player;

void setup() {  
  //setup serial communication
  Serial.begin(9600);

  //init the screen
  Screen* screen = &Screen::Instance();
  screen->Init();
  
  int playerX = (screen->Width() / 2) - (player.GetWidth() / 2);
  int playerY = (screen->Height() / 2) - (player.GetHeight() / 2);
  player.SetPosition(playerX, playerY);
  
  startTime = millis();
}

void movePlayer()
{
  float UpDown = ((analogRead(A7) / 1020.0) - 0.5) * -1;
  float LeftRight = ((analogRead(A6) / 1020.0) - 0.5 ) * -1;

  player.Move(LeftRight, UpDown);
//  Serial.print("X: ");
//  Serial.print(playerX, DEC);
//  Serial.print("  Y: ");
//  Serial.println(playerY, DEC); 
}

void printFPS(int fps, uint64_t color)
{
  //tft.setCursor(1,1);
  //tft.setTextColor(color);
  //tft.setTextSize(1);
  //tft.print(fps, DEC);
  Serial.println(fps);
}

void loop() {  
  count++;
  if (millis() - startTime >= 1000)
  {
    //printFPS(lastFPS, bgcolor);
    lastFPS = count;
    printFPS(lastFPS, ST7735_WHITE);
    
    startTime = millis();
    count = 0;
  }
  
  
  movePlayer();   
   //delay(10);  
}
