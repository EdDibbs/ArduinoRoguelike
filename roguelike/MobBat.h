#ifndef __MOBBAT_h
#define __MOBBAT_h
#include <Arduino.h>
#include "Actor.h"

class MobBat : public Actor
{
  public:
  MobBat(Level* curLevel);
  
  void Update();
  void UpdateAnimationFrame(uint8_t dir);
  void OnActorCollision(Actor* other);
  


  private:
  
};

#endif
