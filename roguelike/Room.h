#ifndef __ROOM_h
#define __ROOM_h

#include "Actor.h"

struct Unit
{
  Actor* actor;
  
  Unit* next;
  Unit* prev;
};

enum TileType: byte{
  WallNorth,
  WallEast,
  WallSouth,
  WallWest,
  DoorNorth,
  DoorEast,
  DoorSouth,
  DoorWest,
  FloorNormal,
  FloorAlt,
  FloorHole,
  FloorRock,
  CornerNW,
  CornerNE,
  CornerSW,
  CornerSE,
};

struct Room
{
  static const int GRID_WIDTH = 13;
  static const int GRID_HEIGHT = 8;
  
  Room* NorthNeighbor;
  Room* EastNeighbor;
  Room* SouthNeighbor;
  Room* WestNeighbor;

  Unit* cells[GRID_WIDTH][GRID_HEIGHT];  
  TileType* Tiles;
  bool FlaggedForDeletion;
};

#endif
