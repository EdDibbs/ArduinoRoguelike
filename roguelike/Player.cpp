#include "Player.h"
#include "Screen.h"
#include "SpriteDefinitions.h"

Player::Player()
{
  Width = (short)pgm_read_word_near(PC_WIDTH);
  Height = (short)pgm_read_word_near(PC_HEIGHT);
  MoveSpeed = 10;
  
  Pixels = new int[Width * Height];

  //load the player sprite into memory
  for (int i = 0; i < Width * Height; i++)
  {
    Pixels[i] = (int)pgm_read_word_near(PC + i);
  }
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
  if (CurPosY < 0) CurPosY = 0;

  Update();
}

void Player::Draw()
{
  Screen::Instance().Draw(CurPosX, CurPosY, Width, Height, Pixels);
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
  }

  Draw();
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

