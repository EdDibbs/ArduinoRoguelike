#ifndef __PLAYER_h
#define __PLAYER_h
#include <Arduino.h>
#include "Actor.h"

class Player : public Actor
{
  public:
    Player();

    void Move(float xDir, float yDir);
    void Draw();
    void DrawFromDisk();
    void Undraw();
    void Update();
    void UpdateAnimationFrame(uint8_t dir);
    void OnActorCollision(Actor* other);
    void SetPosition(int xpos, int ypos);

    int HP;
    int MaxHP;
    
  private:
    const static int MoveSpeed = 10;
    
    
};

#endif
