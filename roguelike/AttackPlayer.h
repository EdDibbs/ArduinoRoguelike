#ifndef __ATTACKPLAYER_h
#define __ATTACKPLAYER_h
#include "Actor.h"

class AttackPlayer : public Actor
{
  public:
  
  AttackPlayer(Level* curLevel, uint8_t dir);
  ~AttackPlayer();

  void Update();
  void UpdateAnimationFrame(uint8_t dir);
  void OnActorCollision(Actor* other);

  private:  
  uint8_t Direction;
  unsigned long TimeToKill;
  
};

#endif
