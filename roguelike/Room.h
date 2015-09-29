#ifndef __ROOM_h
#define __ROOM_h

#include "Actor.h"

struct Unit
{
  Actor* actor;
  
  Unit* next;
  Unit* prev;
};

struct PlayGrid
{
  static const int GRID_WIDTH = 13;
  static const int GRID_HEIGHT = 8;

  Unit* cells[GRID_WIDTH][GRID_HEIGHT];
};

struct Room
{
  Room* NorthNeighbor;
  Room* EastNeighbor;
  Room* SouthNeighbor;
  Room* WestNeighbor;

  PlayGrid* Grid;  
  
  bool FlaggedForDeletion;
};

#endif
