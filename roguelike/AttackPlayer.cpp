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
      Serial.print("Got unexpected direction in AttackPlayer() : ");
      Serial.println(dir);
    break;
  }

  Width = (short)pgm_read_word_near(CurSpritePtr);
  Height = (short)pgm_read_word_near(CurSpritePtr + 1);
  MoveSpeed = 2;
  
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
  
}

void AttackPlayer::OnActorCollision(Actor* other)
{
  if (other->Type != TypeMob) return;
  
  //do damage to other actor
  other->HP = other->HP - 1;
  
  //if we're not piercing, just stop updating and release
}


