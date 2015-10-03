#include "Player.h"
#include "Screen.h"
#include "SpriteDefinitions.h"

Player::Player()
{
  LastSpritePtr = PC_DOWN;
  CurSpritePtr = PC_DOWN;
  CurAnimationState = StandDown;
  NumFramesPerAnim = 10;
  CurAnimationCount = 0;
  
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
    for ( int x = 0; x < LastWidth; x++)
  {
    for (int y = 0; y < LastHeight; y++)
    {
      int index = (y * LastWidth) + x;

      //add 2 for the width and height values
      int value = (int)pgm_read_word_near(LastSpritePtr + 2 + index);

      //because we're using all possible values of a uint16, we need to specify a 
      //color that will denote transparency. In this case, it's the equivalent of
      //magenta. These values will be ignored (not drawn).
      if (value != 0xF81F)
      {
        Screen::Instance().DrawPixel(LastPosX + x, LastPosY + y, 0x00);
      }  
    }
  }
  //Screen::Instance().DrawRect(LastPosX, LastPosY, LastWidth, LastHeight, 0x00);
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
  
  LastSpritePtr = CurSpritePtr;
  UpdateAnimationFrame(newDir);
  LastDir = newDir;
  
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

void Player::UpdateAnimationFrame(uint8_t dir)
{
  if (LastDir == dir)
  {
    CurAnimationCount++;
    //check if enough frame passed to rerender
    if (CurAnimationCount < NumFramesPerAnim)
    {
      return;
    }
  }
  
  LastSpritePtr = CurSpritePtr;    
  switch (dir)
  {
    case 0: //up
      if (CurPosX == LastPosX && CurPosY == LastPosY) CurSpritePtr = PC_UP;
      else if (CurSpritePtr == PC_WALK_UP_1) CurSpritePtr = PC_WALK_UP_2;
      else if (CurSpritePtr == PC_WALK_UP_2) CurSpritePtr = PC_WALK_UP_3;
      else CurSpritePtr = PC_WALK_UP_1;
      
      CurAnimationState = WalkUp;
      break;
      
    case 1: //right
      if (CurPosX == LastPosX && CurPosY == LastPosY) CurSpritePtr = PC_RIGHT;
      else if (CurSpritePtr == PC_WALK_RIGHT_1) CurSpritePtr = PC_WALK_RIGHT_2;
      else if (CurSpritePtr == PC_WALK_RIGHT_2) CurSpritePtr = PC_WALK_RIGHT_3;
      else CurSpritePtr = PC_WALK_RIGHT_1; 
      
      CurAnimationState = WalkRight; 
      break;
      
    case 2: //down
      if (CurPosX == LastPosX && CurPosY == LastPosY) CurSpritePtr = PC_DOWN;
      else if (CurSpritePtr == PC_WALK_DOWN_1) CurSpritePtr = PC_WALK_DOWN_2;
      else if (CurSpritePtr == PC_WALK_DOWN_2) CurSpritePtr = PC_WALK_DOWN_3;
      else CurSpritePtr = PC_WALK_DOWN_1; 
      
      CurAnimationState = WalkDown;
      break;
      
    case 3: //left
      if (CurPosX == LastPosX && CurPosY == LastPosY) CurSpritePtr = PC_LEFT;
      else if (CurSpritePtr == PC_WALK_LEFT_1) CurSpritePtr = PC_WALK_LEFT_2;
      else if (CurSpritePtr == PC_WALK_LEFT_2) CurSpritePtr = PC_WALK_LEFT_3;
      else CurSpritePtr = PC_WALK_LEFT_1; 
      
      CurAnimationState = WalkLeft;
      break;
  }

  CurAnimationCount = 0;
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

