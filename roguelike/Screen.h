#ifndef __SCREEN_h
#define __SCREEN_h

class Screen
{
  public:
    static Screen& Instance()
    {
      static Screen instance;
      return instance;
    }
    
    void Init();
    void Draw(short xpos, short ypos, short width, short height, int* pixels);
    void DrawPixel(short xpos, short ypos, int pixel);
    void DrawRect(short xpos, short ypos, short width, short height, int color);
    int Width();
    int Height();
    
    //going to need something here to keep track of the BG tiles

  private:
    Screen();
    Screen(Screen const&);
    void operator=(Screen const&);    
};
#endif
