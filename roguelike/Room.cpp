#include <Arduino.h>
#include "Room.h"
#include "Actor.h"


Room::Room()
{
  NorthNeighbor = NULL;
  EastNeighbor = NULL;
  SouthNeighbor = NULL;
  WestNeighbor = NULL;
}

Room::~Room()
{
  for (int x = 0; x < GRID_WIDTH; x++)
  {
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
      while(cells[x][y] != NULL)
      {
        Unit* temp = cells[x][y]->next;

        if (cells[x][y]->actor->Type != TypePlayer)
        {
          delete cells[x][y];
        }        

        cells[x][y] = temp;
      }
      
    }
  }

  if (Tiles != NULL)
  {
    delete[] Tiles;
    Tiles = NULL;
  }
}

