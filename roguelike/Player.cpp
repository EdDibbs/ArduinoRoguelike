#include "Player.h"
#include "Screen.h"
#include "SpriteDefinitions.h"

Player::Player()
{
  Width = (short)pgm_read_word_near(PC2_DOWN_WIDTH);
  Height = (short)pgm_read_word_near(PC2_DOWN_HEIGHT);
  MoveSpeed = 10;

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
  Screen::Instance().Draw(CurPosX, CurPosY, Width, Height, Pixels);
}

void Player::DrawFromDisk()
{
  for ( int x = 0; x < Width; x++)
  {
    for (int y = 0; y < Height; y++)
    {
      int index = (y * Width) + x;

      int value = (int)pgm_read_word_near(PC2_DOWN + index);;

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
  Screen::Instance().DrawRect(LastPosX, LastPosY, Width, Height, COLOR_BG);
}

void Player::Update()
{
  if (CurPosX != LastPosX || CurPosY != LastPosY)
  {
    Undraw();
    DrawFromDisk();
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

