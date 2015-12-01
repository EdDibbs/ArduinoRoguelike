#ifndef __ROOM_h
#define __ROOM_h
#include "Macros.h"
#include <Arduino.h>

class Actor;

struct Unit
{
  ~Unit(){} 
  
  Actor* actor;
  
  Unit* next;
  Unit* prev;
};

enum TileType{
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

class Room
{
  public:
  Room();
  ~Room();
  static const int GRID_WIDTH = 13;
  static const int GRID_HEIGHT = 8;
  
  Room* NorthNeighbor;
  Room* EastNeighbor;
  Room* SouthNeighbor;
  Room* WestNeighbor;

  Unit* cells[GRID_WIDTH][GRID_HEIGHT];  
  TileType* Tiles;
  bool FlaggedForDeletion;
  bool PlayerVisited = false;
  bool DrawVisited = false;

  void RemoveActor(uint32_t id);

  private:
  void PrintCell(Unit* head);
};

#endif
