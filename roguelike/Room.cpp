#include <Arduino.h>
#include "Macros.h"
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
  Sprintln("Destructing room...");
  
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

void Room::PrintCell(Unit* head)
{
  Unit* iter = head;
  while (iter != NULL)
  {      
    Sprint((unsigned int) iter);  
    iter = iter->next;
    if (iter != NULL)
      Sprint("->");
  }
  
  Sprintln();
}

void Room::RemoveActor(uint32_t id)
{
  int count = 0;
  
  for (int x = 0; x < GRID_WIDTH; x++)
  {
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
      Unit* unit = cells[x][y];
      while(unit != NULL)
      {
        if(unit->actor->UniqueId == id)
        {        
          //break the link to this actor
          if (unit->next != NULL)
          {
            unit->next->prev = unit->prev;
          }
          
          if (unit->prev != NULL)
          {
            unit->prev->next = unit->next;
          }
          else
          {
            cells[x][y] = unit->next;
          }
          
          //free the memory          
          delete unit; 
          unit = NULL; 
          count++;

          //return;
        }

        unit = unit->next;        
      }
     
    }
  }
  
  Sprint("Removed ");
  Sprint(count);
  Sprintln(" instances of actor.");
  Serial.flush();
  
}

