#ifndef __LEVEL_h
#define __LEVEL_h

#include "Room.h"

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

  int* FloorTile;
  int* NorthWallTile;
  int* EastWallTile;
  int* SouthWallTile;
  int* WestWallTile;

  int* NorthDoorTile;
  int* EastDoorTile;
  int* SouthDoorTile;
  int* WestDoorTile; 

  private:
  void ReleaseRoom(Room* room); //recursively releases room resources
};

#endif
