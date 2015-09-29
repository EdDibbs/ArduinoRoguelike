#ifndef __PLAYER_h
#define __PLAYER_h

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
    void OnActorCollision(Actor* other);
    void SetPosition(int xpos, int ypos);

    int HP;
    int MaxHP;
    
  private:
    int MoveSpeed;
};

#endif
