#ifndef __PLAYER_h
#define __PLAYER_h
#include <Arduino.h>
#include "Actor.h"

class Player : public Actor
{
  public:
    Player(Level* curLevel);

    void Update();
    void UpdateAnimationFrame(uint8_t dir);
    void OnActorCollision(Actor* other);


    
  private:
    void pollJoysticks();
    void attack(uint8_t dir);
    long invulnTimeRemaining;
    long lastInvulnTick;
    short invulnFlashCount;
    bool invulnInvisible = false;
    
};

#endif
