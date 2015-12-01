#ifndef __LEVEL_h
#define __LEVEL_h

#include "Room.h"
#include <Arduino.h>

#define LevelDrawYOffset 30
#define LevelDrawXOffset 2

#define LevelWidth 13
#define LevelHeight 8

#define TileWidth 12
#define TileHeight 12

#define LevelMinX LevelDrawXOffset
#define LevelMinY LevelDrawYOffset
#define LevelMaxX LevelWidth * TileWidth + LevelMinX
#define LevelMaxY LevelHeight* TileHeight+ LevelMinY

#define NorthDoorX LevelDrawXOffset + ((LevelWidth / 2) * TileWidth)
#define NorthDoorY LevelDrawYOffset

#define SouthDoorX NorthDoorX
#define SouthDoorY LevelDrawYOffset + ((LevelHeight - 1) * TileHeight)

#define WestDoorX LevelDrawXOffset
#define WestDoorY LevelDrawYOffset + ((LevelHeight / 2) * TileHeight)

#define EastDoorX LevelDrawXOffset + ((LevelWidth - 1) * TileWidth)
#define EastDoorY WestDoorY

enum LevelType
{
  Jungle,
  Stone
};

class Level
{
  public:
  Level(LevelType type);
  ~Level();
  
  LevelType Type;
  Room* EntryRoom;
  Room* CurrentRoom;

  uint16_t* GetTilePixelsCopyByType(TileType type);
  
  uint16_t* FloorTile;
  uint16_t* NorthSouthWallTile;
  uint16_t* WestEastWallTile;
  uint16_t* CornerTile;
  uint16_t* NorthSouthDoorTile;
  uint16_t* WestEastDoorTile;
  
  void DrawLevel();
  void UnlockDoors();
  private:
  void ReleaseRoom(Room* room); //recursively releases room resources
  
  void PopulateRoomWithMobs();
  Room* GenerateTestRoom();
  
  void LoadJungleSprites();
  void LoadStoneSprites();
};

#endif
