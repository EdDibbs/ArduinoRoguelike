#include "Macros.h"
#include "AttackPlayer.h"
#include "Actor.h"
#include "SpriteDefinitions.h"

AttackPlayer::AttackPlayer(Level* curLevel, uint8_t dir) : Actor(curLevel)
{
  Type = TypePlayerProjectile;
  Direction = dir;
  switch (dir)
  {
    case 0: //up
      CurSpritePtr = ATTACK_UP;
    break;
    case 1: //right
      CurSpritePtr = ATTACK_RIGHT;
    break;
    case 2: //down
      CurSpritePtr = ATTACK_DOWN;
    break;
    case 3: //left
      CurSpritePtr = ATTACK_LEFT;
    break;
    default:
      Sprint("Got unexpected direction in AttackPlayer() : ");
      Sprintln(dir);
    break;
  }

  Width = (short)pgm_read_word_near(CurSpritePtr);
  Height = (short)pgm_read_word_near(CurSpritePtr + 1);
  MoveSpeed = 2;
  NumFramesPerAnim = 10;
  CurAnimationCount = 0;
  
  TimeToKill = millis() + 1000;
}

AttackPlayer::~AttackPlayer()
{
  
}

void AttackPlayer::Update()
{
  //move in our direction, count down to fizzle
  if (millis() >= TimeToKill)
  {        
    Undraw();    
    FlaggedForDeletion = true;
    return;
  }

  switch (Direction)
  {
    case 0: //up
      Move(0, -1);
    break;
    case 1: //right
      Move(1, 0);
    break;
    case 2: //down
      Move(0, 1);
    break;
    case 3: //left
      Move(-1, 0);
    break;
  }

}

void AttackPlayer::UpdateAnimationFrame(uint8_t dir)
{
  CurAnimationCount++;
  //check if enough frame passed to rerender
  if (CurAnimationCount < NumFramesPerAnim)
  {
    return;
  }
  
  switch (Direction)
  {
    case 0: //up
    
    break;
    case 1: //right
      if (CurSpritePtr == ATTACK_RIGHT) CurSpritePtr = ATTACK_DIAG_D_RIGHT;
      else if (CurSpritePtr == ATTACK_DIAG_D_RIGHT) CurSpritePtr = ATTACK_DOWN;
      else if (CurSpritePtr == ATTACK_DOWN) CurSpritePtr = ATTACK_DIAG_D_LEFT;
      else if (CurSpritePtr == ATTACK_DIAG_D_LEFT) CurSpritePtr = ATTACK_LEFT;
      else if (CurSpritePtr == ATTACK_LEFT) CurSpritePtr = ATTACK_DIAG_UP_LEFT;
      else if (CurSpritePtr == ATTACK_DIAG_UP_LEFT) CurSpritePtr = ATTACK_UP;
      else if (CurSpritePtr == ATTACK_UP) CurSpritePtr = ATTACK_DIAG_UP_RIGHT;
      else CurSpritePtr = ATTACK_RIGHT;
    break;
    case 2: //down
      
    break;
    case 3: //left
      if (CurSpritePtr == ATTACK_LEFT) CurSpritePtr = ATTACK_DIAG_D_LEFT;
      else if (CurSpritePtr == ATTACK_DIAG_D_LEFT) CurSpritePtr = ATTACK_DOWN;
      else if (CurSpritePtr == ATTACK_DOWN) CurSpritePtr = ATTACK_DIAG_D_RIGHT;
      else if (CurSpritePtr == ATTACK_DIAG_D_RIGHT) CurSpritePtr = ATTACK_RIGHT;
      else if (CurSpritePtr == ATTACK_RIGHT) CurSpritePtr = ATTACK_DIAG_UP_RIGHT;
      else if (CurSpritePtr == ATTACK_DIAG_UP_RIGHT) CurSpritePtr = ATTACK_UP;
      else if (CurSpritePtr == ATTACK_UP) CurSpritePtr = ATTACK_DIAG_UP_LEFT;
      else CurSpritePtr = ATTACK_LEFT;
    break;
  }
  
  CurAnimationCount = 0;
}

void AttackPlayer::OnActorCollision(Actor* other)
{
  if (FlaggedForDeletion) return;
  if (other->Type != TypeMob) return;
  
  //do damage to other actor
  other->HP = other->HP - 1;
  
  //if we're not piercing, just stop updating and release
  FlaggedForDeletion = true;
}


