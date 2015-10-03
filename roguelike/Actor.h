#ifndef __ACTOR_h
#define __ACTOR_h
#include <Arduino.h>
class Actor
{
  public:
    Actor() {};
  
    virtual void Draw() = 0;
    virtual void Undraw() = 0;
    virtual void Update() = 0;
    virtual void OnActorCollision(Actor* other) = 0;

    short GetWidth()  {return Width;}
    short GetHeight() {return Height;}
  protected:
    int CurPosX;
    int LastPosX;
    
    int CurPosY;
    int LastPosY;

    uint8_t LastDir; //0 = up, 1 = right, 2 = down, 3 = left
    bool FlipHorizontalDraw = false;
    
    uint8_t Width;
    uint8_t Height;
    uint8_t LastWidth;
    uint8_t LastHeight;
    
    int* Pixels;

    const uint16_t* CurSpritePtr;
    const uint16_t* LastSpritePtr;
};

#endif
