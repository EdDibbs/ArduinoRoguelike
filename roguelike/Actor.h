#ifndef __ACTOR_h
#define __ACTOR_h
#include <Arduino.h>
#include "Level.h"

class Level;
static uint32_t NextUniqueId = 0;
static uint32_t GetUniqueId(){
  return NextUniqueId++;
  }

enum ActorType{
  TypePlayer,
  TypeMob,
  TypePlayerProjectile,
  TypeMobProjectile,
  TypeMobProjectileHitAll
};

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
    Actor(Level* curLevel) ;
    ~Actor() { Serial.print("~Actor() called for : "); Serial.println(UniqueId); };
    ActorType Type;
    
    void Draw();
    void Undraw();
    virtual void Update() = 0;
    void UpdateMovement();
    void UpdatePlaygridLoc();
    virtual void UpdateAnimationFrame(uint8_t dir) {};
    void Move(float xDir, float yDir);
    void SetPosition(int xpos, int ypos);
    
    virtual void OnActorCollision(Actor* other) = 0;

    short GetWidth()  {return Width;}
    short GetHeight() {return Height;}
    short GetTileX() { return (CurPosX - LevelDrawXOffset) / TileWidth; }    
    short GetTileY() { return (CurPosY - LevelDrawYOffset) / TileHeight;}

    int CurPosX;
    int LastPosX;
    
    int CurPosY;
    int LastPosY;    

    uint8_t Width;
    uint8_t Height;

    const uint16_t* CurSpritePtr;
    const uint16_t* LastSpritePtr;
    
    int HP;
    int MaxHP;    

    uint32_t UniqueId;
    bool MovedThisFrame = false;
    bool FlaggedForDeletion = false;
  protected:
    Level* CurLevel; //used for rendering actors on top of the terrian correctly
    
    int MoveSpeed;
    
    AnimationState CurAnimationState;
    uint8_t LastDir; //0 = up, 1 = right, 2 = down, 3 = left
    bool FlipHorizontalDraw = false;
    

    uint8_t LastWidth;
    uint8_t LastHeight;
    uint8_t NumFramesPerAnim;
    uint8_t CurAnimationCount;    
    
    
};

#endif
