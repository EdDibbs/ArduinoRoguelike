#include "Actor.h"

class Player : public Actor
{
  public:
    Player();

    void Move(float xDir, float yDir);
    void Draw();
    void Undraw();
    void Update();
    void OnActorCollision(Actor* other);
    void SetPosition(int xpos, int ypos);
    
  private:
    int MoveSpeed;
};

