#include <avr/pgmspace.h>
#include <Adafruit_GFX.h>   //Core graphics library
#include <Adafruit_ST7735.h> //hardware-specific library
#include <SPI.h> //communication for screen
#include "SpriteDefinitions.h"
#include "Screen.h"
#include "Actor.h"
#include "Player.h"
#include "Level.h"
#include "Room.h"
#include "MemoryFree.h"

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
  const short heartWidth = (short)pgm_read_word_near(HP_WIDTH);
  const short heartHeight = (short)pgm_read_word_near(HP_HEIGHT);

  //load the images  
  int* pixelsFullHeart = new int[heartWidth* heartHeight];
  for (int i = 0; i < heartWidth* heartHeight; i++)
  {
    pixelsFullHeart[i] = pgm_read_word_near(HP + i);  
  }

  int* pixelsEmptHeart = new int[heartWidth * heartHeight];
  for (int i = 0; i < heartWidth* heartHeight; i++)
  {
    pixelsEmptHeart[i] = pgm_read_word_near(HP_EMPTY + i);  
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
  Screen* screen = &Screen::Instance();
  screen->tft.setCursor(1,9);
  screen->tft.setTextColor(color);
  screen->tft.setTextSize(1);
  screen->tft.print(freeMem, DEC);
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
  Screen* screen = &Screen::Instance();
  screen->tft.setCursor(1,1);
  screen->tft.setTextColor(color);
  screen->tft.setTextSize(1);
  screen->tft.print(fps, DEC);
  //Serial.print("FPS: ");
  //Serial.println(fps);
}
