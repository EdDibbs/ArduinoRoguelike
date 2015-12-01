#ifndef __PLAYER_h
#define __PLAYER_h
#include <Arduino.h>
#include "Macros.h"
#include "Actor.h"

#define JOYSTICK_LEFT_X A2
#define JOYSTICK_LEFT_Y A3
#define JOYSTICK_RIGHT_X A4
#define JOYSTICK_RIGHT_Y A5

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
    long attackCooldown;
    
    long invulnTimeRemaining;
    long lastInvulnTick;
    short invulnFlashCount;
    bool invulnInvisible = false;
    
};

#endif
