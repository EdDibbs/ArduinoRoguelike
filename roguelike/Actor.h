#ifndef __ACTOR_h
#define __ACTOR_h
#include <Arduino.h>

class Level;
static uint32_t NextUniqueId = 0;
static uint32_t GetUniqueId(){
  return NextUniqueId++;
  }
  
enum AnimationState{
  StandDown,
  StandLeft,
  StandRight,
  StandUp,
  WalkDown,
  WalkLeft,
  WalkRight,
  WalkUp,
  AttackDown,
  AttackLeft,
  AttackRight,
  AttackUp
};

class Actor
{
  public:
    Actor(Level* curLevel) {CurLevel = curLevel; UniqueId = GetUniqueId();};
    
    void Draw();
    void Undraw();
    virtual void Update() = 0;
    void UpdateMovement();
    void UpdatePlaygridLoc();
    virtual void UpdateAnimationFrame(uint8_t dir) {};
    void Move(float xDir, float yDir);
    
    virtual void OnActorCollision(Actor* other) = 0;

    short GetWidth()  {return Width;}
    short GetHeight() {return Height;}
    
  protected:
    Level* CurLevel; //used for rendering actors on top of the terrian correctly
    int CurPosX;
    int LastPosX;
    
    int CurPosY;
    int LastPosY;
    int MoveSpeed;
    
    AnimationState CurAnimationState;
    uint8_t LastDir; //0 = up, 1 = right, 2 = down, 3 = left
    bool FlipHorizontalDraw = false;
    
    uint8_t Width;
    uint8_t Height;
    uint8_t LastWidth;
    uint8_t LastHeight;
    uint8_t NumFramesPerAnim;
    uint8_t CurAnimationCount;    
    
    const uint16_t* CurSpritePtr;
    const uint16_t* LastSpritePtr;

    uint32_t UniqueId;
};

#endif
