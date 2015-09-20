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
    short CurPosX;
    short LastPosX;
    
    short CurPosY;
    short LastPosY;
    
    short Width;
    short Height;
    int* Pixels;
};

