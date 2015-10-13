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
  Width = (short)pgm_read_word_near(CurSpritePtr);
  Height = (short)pgm_read_word_near(CurSpritePtr + 1);
  Type = TypePlayer;
  invulnTimeRemaining = 0.0;
  
  HP = 3;
  MaxHP = 3;
  MoveSpeed = 5;
}

void Player::Update()
{
  //non-movement logic goes here
  if (invulnTimeRemaining > 0.0)
  {
    invulnTimeRemaining -= millis() - lastInvulnTick;
    lastInvulnTick = millis();
    invulnFlashCount--;
    if (invulnFlashCount == 0)
    {
      invulnFlashCount = 5;
      invulnInvisible = !invulnInvisible;
    }
    
    if (invulnTimeRemaining <= 0.0)
    {
      invulnTimeRemaining = 0.0;
      invulnInvisible = false;
    }
  }

}

void Player::UpdateAnimationFrame(uint8_t dir)
{
  if (invulnInvisible)
  {
    Serial.println("Invis");
    LastSpritePtr = CurSpritePtr;    
    CurSpritePtr = INVISIBLE;
    return;
  }
  
  if (LastDir == dir && CurSpritePtr != INVISIBLE)
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

    case 255: //haven't moved
      switch(LastDir)
      {
        case 0: //up
          CurSpritePtr = PC_WALK_UP_1;          
          CurAnimationState = StandUp;
          break;
          
        case 1: //right
          CurSpritePtr = PC_WALK_RIGHT_1;       
          CurAnimationState = StandRight; 
          break;
          
        case 2: //down
          CurSpritePtr = PC_WALK_DOWN_1;       
          CurAnimationState = StandDown;
          break;
          
        case 3: //left
          CurSpritePtr = PC_WALK_LEFT_1;       
          CurAnimationState = StandLeft;
          break;
      }
    break;
  }

  
  CurAnimationCount = 0;  
}

void Player::OnActorCollision(Actor* other)
{
  if(other->Type == TypeMob && invulnTimeRemaining == 0.0)
  {
    HP--;
    invulnTimeRemaining = 3000; //3 seconds
    lastInvulnTick = millis();
    invulnFlashCount = 5;
  }
}


