#include "Macros.h"
#include <Arduino.h>
#include <avr/pgmspace.h> //abililty to store variables in program space

//////////////////////////////////////
//Screen stuff
#include "PDQ_ST7735_config.h" //screen configuration
#include <PDQ_FastPin.h>
#include <PDQ_GFX.h>   //Core graphics library
#include <PDQ_ST7735.h> //hardware-specific library
#include <SPI.h> //communication for screen
#include "SpriteDefinitions.h"
#include "Screen.h"
//
///////////////////////////////////////

#include "Actor.h"
#include "Player.h"
#include "Level.h"
#include "Room.h"
#include "MemoryFree.h"

///////////////////////////////////////
//Enemies
#include "MobBat.h"

//
///////////////////////////////////////


PDQ_ST7735 _tft; //global access to the screen needed by the library


//clock stuff
long startTime;
unsigned long count = 0;
unsigned long lastFPS = 0;

bool isRunning = false;
bool lostGame = false;
long lastInputMeasure;

int lastHPdraw = -1;
int lastFreeMem = 0;


Level* CurrentLevel;
Player* player;

void setup() {  
  //sleep for a second in case we need to do something
  delay(500);
  
  //setup serial communication  
  Serial.begin(115200);
  Sprintln(F("Setting up..."));
  
  //init the screen
  Screen* screen = &Screen::Instance();
  screen->Init();
  
  
  StartNewGame();
}


void loop() {
  
  if (isRunning)
  {     
    measureFPS();
    
    drawHearts();
  
    if (millis() - lastInputMeasure >= 1000/60)
    {
      measureFreeMem();      
      updateRoom();
      lastInputMeasure = millis();
    }

    if (player->HP <= 0)
    {
      isRunning = false;
      lostGame = true;
    }
  }

  if (lostGame)
  {
    Sprintln(F("Game Over!"));
    printGameOverScreen();
    lostGame = false; //don't keep drawing this text

    delay(1000);    
    StartNewGame();
    //start new game currently broken
  }
}

void ShowSplashScreen()
{
  _tft.fillScreen(ST7735_BLACK);
  uint16_t splashWidth = SPLASHSCREEN[0];
  uint16_t splashHeight = SPLASHSCREEN[1];
  
  for (int i = 6; i >= 0; i--)
  {    
    uint16_t curcolor = 0;
    uint16_t color = _tft.color565((1 << i) - 1, (1 << i) - 1, (1 << i) - 1);
    
    uint16_t cnt = 0;    
    
    while (cnt < splashWidth * splashHeight)
    {
      curcolor = (uint16_t)pgm_read_word_near(SPLASHSCREEN + 2 + cnt);
      _tft.drawRect((cnt % splashWidth) * 2, (cnt / splashWidth) * 2, 2, 2, curcolor | color);
      cnt++;
    }
  }

    delay(5000);
  __FillScreen(0x0000);

  __SetCursor(3,3);
  __SetTextWrap(true);
  _tft.print(F(""));
}

void StartNewGame()
{
  Sprintln(F("Starting new game..."));
  PrintStartGameText();
  ShowSplashScreen();  
  


  SwitchLevel(Jungle);
  
  player = new Player(CurrentLevel);
    
  //draw status frame
  __DrawRect(0,0,160,24,ST7735_BLACK); 
  lastInputMeasure = startTime = millis();
  isRunning = true;
  lostGame = false;
}

bool CheckForCollision(Actor* act1, Actor* act2)
{
//  Sprint("Checking ");
//  Sprint(act1->UniqueId);
//  Sprint(" against ");
//  Sprint(act2->UniqueId);
//  Sprint("...");
  
  //AABB checking
  if (act1->CurPosX + act1->Width > act2->CurPosX
      && act1->CurPosX < act2->CurPosX + act2->Width
      && act1->CurPosY + act1->Height > act2->CurPosY
      && act1->CurPosY < act2->CurPosY + act2->Height)
      {
        //Sprintln("true");
        return true;
      }

  //Sprintln("false");
  return false;
}

void updateRoom()
{
  int mobCount = 0;
  
  unsigned long startTime = millis();
  Room* room = CurrentLevel->CurrentRoom;
  int updateCount = 0;
  
  for (int x = 0; x < LevelWidth; x++)
  {    
    for (int y = 0; y < LevelHeight; y++)
    {       
//      Sprint("Checking (");
//      Sprint(x);
//      Sprint(", ");
//      Sprint(y);
//      Sprintln(")...");
//      Serial.flush(); 
//      
      if (room == NULL)
      {
        Sprintln(F("ROOM IS NULL IN updateRoom()"));        
        Serial.flush();
        continue;
      }
      
      //update all units
      Unit* units = room->cells[x][y];
      while (units != NULL)
      {              
        Actor* actor = units->actor;
        if (actor == NULL)
        {
          Sprintln(F("FOUND A UNIT WITH ACTOR == NULL"));
          Serial.flush();
          units = units->next;
          continue;
        }
        
        actor->MovedThisFrame = false;
        actor->Update();
        updateCount++;
        if (actor->Type == TypeMob)
        {
          mobCount++;
        }

        units = units->next;
        if (actor->FlaggedForDeletion)
        {       
          actor->Undraw();          
          room->RemoveActor(actor->UniqueId);
          
          delete actor;
          actor = NULL;
        }     

      }
    }
  }
  
  for (int x = 0; x < LevelWidth; x++)
  {    
    for (int y = 0; y < LevelHeight; y++)
    {          
      //update all units
      Unit* units = room->cells[x][y];
      while (units != NULL)
      {  
        Actor* actor = units->actor;                
        if (actor == NULL)
        {
          Sprintln(F("FOUND A UNIT WITH ACTOR == NULL"));
          Serial.flush();
          units = units->next;
          continue;
        }
        
        if (!actor->MovedThisFrame)
        {
          actor->UpdatePlaygridLoc();
          actor->MovedThisFrame = true;
        }

//        Sprint(actor->UniqueId);
//        Sprint(" is in [");
//        Sprint(x);
//        Sprint(", ");
//        Sprint(y);
//        Sprintln("]");
        
        for (int otherX = x - 1; otherX < x + 2; otherX++)
        {
          if (otherX < 0 || otherX >= LevelWidth) continue;
          for (int otherY = y - 1; otherY < y + 2; otherY++)
          {
            if (otherY < 0 || otherY >= LevelHeight) continue;
            
            Unit* otherUnits = room->cells[otherX][otherY];
            while (otherUnits != NULL)
            {
              //do we collide with this actor?
              if (actor->UniqueId != otherUnits->actor->UniqueId && CheckForCollision(actor, otherUnits->actor))
              {
                actor->OnActorCollision(otherUnits->actor);
              }
              
              otherUnits = otherUnits->next;
            }
          }
        }
        
        
        units = units->next;
      }
   
    }

  }

  //check if player is on top of a doorway
  if (mobCount == 0)
  {
    CheckForDoorCollisions();
  }
  

  unsigned long timeTaken = millis() - startTime;
//  Sprint(updateCount);
//  Sprintln(" updates called.");
//  Serial.flush();

 
//  Sprint(F("Update room took "));
//  Sprint(timeTaken);
//  Sprintln(F(" ms."));
  
}

void CheckForDoorCollisions()
{
  Room* newRoom = NULL;
  int dir;
  for(dir = 0; dir < 4; dir++)
  {
    int xPos;
    int yPos;
    
    switch (dir)
    {
      case 0: xPos = NorthDoorX; yPos = NorthDoorY; break;
      case 1: xPos = EastDoorX;  yPos = EastDoorY;  break;        
      case 2: xPos = SouthDoorX; yPos = SouthDoorY; break;
      case 3: xPos = WestDoorX;  yPos = WestDoorY;  break;
    }

    if (player->CurPosX + player->Width > xPos
      && player->CurPosX < xPos + TileWidth
      && player->CurPosY + player->Height > yPos
      && player->CurPosY < yPos + TileHeight)
      {        
        switch(dir)
        {
          case 0: newRoom = CurrentLevel->CurrentRoom->NorthNeighbor; break;
          case 1: newRoom = CurrentLevel->CurrentRoom->EastNeighbor; break;
          case 2: newRoom = CurrentLevel->CurrentRoom->SouthNeighbor; break;
          case 3: newRoom = CurrentLevel->CurrentRoom->WestNeighbor; break;
        }

        //break out of this for loop so we retain dir
        break;
      }
  }

  if (newRoom != NULL)
  {
    _tft.fillRect(LevelMinX, LevelMinY, LevelMaxX - LevelMinX, LevelMaxY - LevelMinY, 0x0000); 
    CurrentLevel->CurrentRoom = newRoom;
    CurrentLevel->DrawLevel();

    int newX, newY;
    switch(dir)
    {
      case 0: newX = SouthDoorX; newY = SouthDoorY - player->Height - 5; break;
      case 1: newX = WestDoorX + TileWidth + 5; newY = WestDoorY; break;
      case 2: newX = NorthDoorX; newY = NorthDoorY + TileHeight + 5; break;
      case 3: newX = EastDoorX - 5; newY = EastDoorY; break;
    }

    player->SetPosition(newX, newY);
  }
  
}


void SwitchLevel(LevelType type)
{
  if (CurrentLevel != NULL)
  {
    delete CurrentLevel;
  }

  CurrentLevel = new Level(type);
}

void drawHearts()
{
  if (lastHPdraw == player->HP)
  {
    return;
  }
  lastHPdraw = player->HP;
  
  const int startXpos = 130;
  const int startYpos = 5;
  const int maxHeartsPerRow = 4;
  const short heartWidth = (short)pgm_read_word_near(HP);
  const short heartHeight = (short)pgm_read_word_near(HP + 1);

  //load the images  
  uint16_t* pixelsFullHeart = new uint16_t[heartWidth* heartHeight];
  for (int i = 0; i < heartWidth* heartHeight; i++)
  {
    pixelsFullHeart[i] = pgm_read_word_near(HP + 2 + i);  
  }

  uint16_t* pixelsEmptHeart = new uint16_t[heartWidth * heartHeight];
  for (int i = 0; i < heartWidth* heartHeight; i++)
  {
    pixelsEmptHeart[i] = pgm_read_word_near(HP_EMPTY + 2 + i);  
  }
  
  
  
  //go through all the hearts that we have
  for (int i = 0; i < player->MaxHP; i++)
  {
    int drawXPos = startXpos + ((heartWidth + 1)  * (i % maxHeartsPerRow));
    int drawYPos = startYpos + ((heartHeight + 3) * (i / maxHeartsPerRow));

    uint16_t* pixels = pixelsEmptHeart;

    if (player->HP > i)
    {
      pixels = pixelsFullHeart;
    }
    
    Screen::Instance().Draw(drawXPos, drawYPos, heartWidth, heartHeight, pixels);
  }
  
  delete[] pixelsEmptHeart;
  delete[] pixelsFullHeart;
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

  if (freeMem < 500)
  {
    Sprintln(F("Out of memory!. Holding the program to prevent damage."));
    isRunning = false;
  }
}

void PrintStartGameText()
{
  bool firstDraw = true;
  int fadeCount = 0x0F;
  int width = __ScreenWidth();
  int height = __ScreenHeight();
  __FillScreen(0x0000);
  __SetTextWrap(true);  
  char* startGameText = "In a world ruled by the\nevil sorcerer, Owen\nChestnut, one alchemist\nstands alone...\n\n\nYou must collect the rare\ningredients needed to makethe special brew that can\ntake down Owen Chestnut...\0";
  int len = strlen(startGameText);

  for (int fade = fadeCount; fade >= 0; fade--)
  {
    uint16_t color = _tft.color565((fade << 4) | fade, (fade << 4) | fade, (fade << 4) | fade);
    __SetCursor(5, 20);
    __SetTextColor(color);
    
    for (int i = 0; i < len; i++)
    {
      _tft.print(startGameText[i]);

      float rightYread = analogRead(JOYSTICK_RIGHT_Y);
      float rightXread = analogRead(JOYSTICK_RIGHT_X);
      float rightUpDown = (( (rightYread) / 1020.0) - 0.5) * -1;
      float rightLeftRight = (((rightXread) / 1020.0) - 0.5 );    
      float upDownAbs = rightUpDown > 0 ? rightUpDown : rightUpDown * -1;
      float leftRightAbs = rightLeftRight > 0 ? rightLeftRight: rightLeftRight* -1;
      
      bool fast = false;
      if (upDownAbs > 0.2 || leftRightAbs > 0.2)
      {
        fast = true;
      }

      if (firstDraw)
      {        
        if (!fast)
          delay(100);
        else
          delay(20);
      }
    }
    if (firstDraw)
    {
      delay(3500);
      firstDraw = false;
    }
    delay(300);
  }
}

void printGameOverScreen()
{
  bool firstDraw = true;
  int fadeCount = 0x0F;
  int width = __ScreenWidth();
  int height = __ScreenHeight();
  __FillScreen(0x0000);
 
  delay(1000);
  char* gameOverText = "Game Over...\0";

  for (int fade = fadeCount; fade >= 0; fade--)
  {
    uint16_t color = _tft.color565((fade << 4) | fade, (fade << 4) | fade, (fade << 4) | fade);
    __SetCursor(width/2 - 30, height/2);
    __SetTextColor(color);
    
    for (int i = 0; i < 12; i++)
    {
      _tft.print(gameOverText[i]);

      if (firstDraw)
      {        
        delay(200);        
        if (i > 8) delay(200);        
      }
    }
    if (firstDraw)
    {
      delay(2700);
      firstDraw = false;
    }
    delay(300);
  }
  
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
    
    startTime = millis();
    count = 0;
  }
}

void printFPS(unsigned long fps, uint64_t color)
{
  __SetCursor(1,1);
  __SetTextColor(color);
  _tft.print(fps, DEC);
  
  //Sprint("FPS: ");
  //Sprintln(fps);
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


