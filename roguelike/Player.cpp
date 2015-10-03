#include "Player.h"
#include "Screen.h"
#include "SpriteDefinitions.h"

Player::Player()
{
  LastSpritePtr = PC_DOWN;
  CurSpritePtr = PC_DOWN;
 

  HP = 3;
  MaxHP = 6;
  
  //Pixels = new int[Width * Height];

  //load the player sprite into memory
//  for (int i = 0; i < Width * Height; i++)
//  {
//    Pixels[i] = (int)pgm_read_word_near(SPRITE_PC2 + i);
//  }
}

void Player::Move(float xDir, float yDir)
{
  int screenWidth = Screen::Instance().Width();
  int screenHeight = Screen::Instance().Height();
  
  LastPosX = CurPosX;
  CurPosX = CurPosX + (int)(MoveSpeed * xDir);
  if (CurPosX + Width > screenWidth) CurPosX = screenWidth - Width;
  if (CurPosX < 0) CurPosX = 0;

  LastPosY = CurPosY;
  CurPosY = CurPosY + (int)(MoveSpeed * yDir);
  if (CurPosY + Height > screenHeight) CurPosY = screenHeight - Height;
  if (CurPosY < 24) CurPosY = 24;

  Update();
}


void Player::Draw()
{
  for ( int x = 0; x < Width; x++)
  {
    for (int y = 0; y < Height; y++)
    {
      int index = (y * Width) + x;
      if (FlipHorizontalDraw)
      {
        x = Width - x - 1;
      }
      
      //add 2 for the width and height values
      int value = (int)pgm_read_word_near(CurSpritePtr + 2 + index);

      //because we're using all possible values of a uint16, we need to specify a 
      //color that will denote transparency. In this case, it's the equivalent of
      //magenta. These values will be ignored (not drawn).
      if (value != 0xF81F)
      {
        Screen::Instance().DrawPixel(CurPosX + x, CurPosY + y, value);
      }  
    }
  }
}

void Player::Undraw()
{
//    for ( int x = 0; x < LastWidth; x++)
//  {
//    for (int y = 0; y < LastHeight; y++)
//    {
//      int index = (y * LastWidth) + x;
//
//      //add 2 for the width and height values
//      int value = (int)pgm_read_word_near(LastSpritePtr + 2 + index);
//
//      //because we're using all possible values of a uint16, we need to specify a 
//      //color that will denote transparency. In this case, it's the equivalent of
//      //magenta. These values will be ignored (not drawn).
//      if (value != 0xF81F)
//      {
//        Screen::Instance().DrawPixel(LastPosX + x, LastPosY + y, COLOR_BG);
//      }  
//    }
//  }
  Screen::Instance().DrawRect(LastPosX, LastPosY, LastWidth, LastHeight, 0x00);
}

void Player::Update()
{
  uint8_t newDir = 255;
  bool needToDraw = false;
  if (CurPosX != LastPosX || CurPosY != LastPosY)
  {
    needToDraw = true;

    uint8_t xMovement, yMovement;

    //get the absolute difference between the current and last positions
    xMovement = CurPosX > LastPosX ? CurPosX - LastPosX : LastPosX - CurPosX;
    yMovement = CurPosY > LastPosY ? CurPosY - LastPosY : LastPosY - CurPosY;

    //did we more more in the xDirection?
    if (xMovement > yMovement)
    {
      //did we move left or right?
      newDir = CurPosX > LastPosX ? 1 : 3;
    }
    else
    {
      //did we move up or down?
      newDir = CurPosY > LastPosY ? 2 : 0;
    }
  }
  
  if (newDir != 255 && newDir != LastDir)
  {
    LastSpritePtr = CurSpritePtr;
    
    switch (newDir)
    {
      case 0: CurSpritePtr = PC_UP;   FlipHorizontalDraw = false;  break;
      case 1: CurSpritePtr = PC_LEFT; FlipHorizontalDraw = true;   break;
      case 2: CurSpritePtr = PC_DOWN; FlipHorizontalDraw = false;  break;
      case 3: CurSpritePtr = PC_LEFT; FlipHorizontalDraw = false;  break;
    }
    LastDir = newDir;
  }

  LastWidth = Width;
  LastHeight = Height;
  Width = (short)pgm_read_word_near(CurSpritePtr);
  Height = (short)pgm_read_word_near(CurSpritePtr + 1);

  if(needToDraw)
  {    
      Undraw();
      Draw();
  }
}

void Player::OnActorCollision(Actor* other)
{
  
}

void Player::SetPosition(int xpos, int ypos)
{
  LastPosX = CurPosX;
  LastPosY = CurPosY;
  
  CurPosX = xpos;
  CurPosY = ypos;

  Update();
}

