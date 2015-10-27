#include "MobBat.h"
#include "Screen.h"
#include "SpriteDefinitions.h"
#include "Actor.h"
#include "Level.h"

MobBat::MobBat(Level* curLevel) : Actor(curLevel)
{
  LastSpritePtr = BAT;
  CurSpritePtr = BAT;
  CurAnimationState = StandDown;
  NumFramesPerAnim = 15;
  CurAnimationCount = 0;
  Width = (short)pgm_read_word_near(CurSpritePtr);
  Height = (short)pgm_read_word_near(CurSpritePtr + 1);
  Type = TypeMob;
  
  HP = 1;
  MaxHP = 1;
  MoveSpeed = 3;
}

void MobBat::Update()
{
  //move around, attack player
  short tileX = GetTileX();
  short tileY = GetTileY();

  if (MovingLeft)
  {
    if (tileX < 1)
    {
      MovingLeft = false;
    }
    else
    {
      Move(-0.8, 0);
    }
  }
  else
  {
    if (tileX > LevelWidth - 3)
    {
      MovingLeft = true;
    }
    else
    {
      Move(0.8,0);
    }
  }
  
//  for (int x = tileX - 4; x < tileX + 4; x++)
//  {
//    if (x < 0 || x >= LevelWidth) continue;
//    for (int y = tileY - 4; y < tileY + 4; y++)
//    {
//      if (y < 0 || y >= LevelHeight) continue;
//
//      //search for the player
//      Unit* units = CurLevel->CurrentRoom->cells[x][y];
//      while (units != NULL)
//      {
//        Actor* actor = units->actor;
//        if (actor->Type == TypePlayer)
//        {          
//          //chase the player
//          float xDir = (float)(actor->CurPosX - CurPosX) / 40;
//          float yDir = (float)(actor->CurPosY - CurPosY) / 40;
//
////          Serial.print("Bat moving. xDir: ");
////          Serial.print(xDir);
////          Serial.print(" yDir: ");
////          Serial.println(yDir);
//          Move(xDir, yDir);
//          return;
//        }
//
//        units = units->next;
//      }
//    }
//  }

  if (HP <= 0) FlaggedForDeletion = true;
  
  //we didn't find anything, so don't move
  Move(0,0);  
}

void MobBat::UpdateAnimationFrame(uint8_t dir)
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
      if (CurPosX == LastPosX && CurPosY == LastPosY) CurSpritePtr = BAT_MOVE_UP_1;
      else if (CurSpritePtr == BAT_MOVE_UP_1) CurSpritePtr = BAT_MOVE_UP_2;      
      else CurSpritePtr = BAT_MOVE_UP_1;
      
      CurAnimationState = WalkUp;
      break;
      
    case 1: //right
      if (CurPosX == LastPosX && CurPosY == LastPosY) CurSpritePtr = BAT_MOVE_RIGHT_1;
      else if (CurSpritePtr == BAT_MOVE_RIGHT_1) CurSpritePtr = BAT_MOVE_RIGHT_2;
      else if (CurSpritePtr == BAT_MOVE_RIGHT_2) CurSpritePtr = BAT_MOVE_RIGHT_3;
      else if (CurSpritePtr == BAT_MOVE_RIGHT_3) CurSpritePtr = BAT_MOVE_RIGHT_4;
      else if (CurSpritePtr == BAT_MOVE_RIGHT_4) CurSpritePtr = BAT_MOVE_RIGHT_5;
      else CurSpritePtr = BAT_MOVE_RIGHT_1; 
      
      CurAnimationState = WalkRight; 
      break;
      
    case 2: //down
      if (CurPosX == LastPosX && CurPosY == LastPosY) CurSpritePtr = BAT;
      else if (CurSpritePtr == BAT) CurSpritePtr = BAT_IDLE_1;
      else if (CurSpritePtr == BAT_IDLE_1) CurSpritePtr = BAT_IDLE_2;
      else CurSpritePtr = BAT; 
      
      CurAnimationState = WalkDown;
      break;
      
    case 3: //left
      if (CurPosX == LastPosX && CurPosY == LastPosY) CurSpritePtr = BAT_MOVE_LEFT_1;
      else if (CurSpritePtr == BAT_MOVE_LEFT_1) CurSpritePtr = BAT_MOVE_LEFT_2;
      else if (CurSpritePtr == BAT_MOVE_LEFT_2) CurSpritePtr = BAT_MOVE_LEFT_3;
      else if (CurSpritePtr == BAT_MOVE_LEFT_3) CurSpritePtr = BAT_MOVE_LEFT_4;
      else if (CurSpritePtr == BAT_MOVE_LEFT_4) CurSpritePtr = BAT_MOVE_LEFT_5;
      else CurSpritePtr = BAT_MOVE_LEFT_1; 
      
      CurAnimationState = WalkLeft;
      break;

    case 255: //haven't moved
      if (CurSpritePtr == BAT) CurSpritePtr = BAT_IDLE_1;
      else if (CurSpritePtr == BAT_IDLE_1) CurSpritePtr = BAT_IDLE_2;
      else CurSpritePtr = BAT; 

      CurAnimationState = StandDown;
      break;
  }

  CurAnimationCount = 0;  
}

void MobBat::OnActorCollision(Actor* other)
{
  Serial.println(F("Bat collided!"));
}


