#include "Player.h"
#include "Screen.h"
#include "SpriteDefinitions.h"

Player::Player(Level* curLevel) : Actor(curLevel)
{
  LastSpritePtr = PC_DOWN;
  CurSpritePtr = PC_DOWN;
  CurAnimationState = StandDown;
  NumFramesPerAnim = 10;
  CurAnimationCount = 0;
  
  HP = 3;
  MaxHP = 6;
  MoveSpeed = 5;
}

void Player::Update()
{
  //non-movement logic goes here
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

  UpdateMovement();
}

