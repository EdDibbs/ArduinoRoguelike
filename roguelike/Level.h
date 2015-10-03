#ifndef __LEVEL_h
#define __LEVEL_h

#include "Room.h"
#include <Arduino.h>

#define TileWidth 12
#define TileHeight 12

enum LevelType
{
  Jungle,
};

class Level
{
  public:
  Level(LevelType type);
  ~Level();
  
  LevelType Type;
  Room* EntryRoom;

  uint16_t* FloorTile;
  uint16_t* NorthWallTile;
  uint16_t* EastWallTile;
  uint16_t* SouthWallTile;
  uint16_t* WestWallTile;

  uint16_t* NorthDoorTile;
  uint16_t* EastDoorTile;
  uint16_t* SouthDoorTile;
  uint16_t* WestDoorTile; 

  private:
  void ReleaseRoom(Room* room); //recursively releases room resources
  void TestDraw();
  void LoadJungleSprites();
};

#endif
