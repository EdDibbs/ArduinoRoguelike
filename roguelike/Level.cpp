#define NULL 0
#include "Level.h"

Level::Level(LevelType type)
{
  
}

Level::~Level()
{
  delete[] FloorTile;
  delete[] NorthWallTile;
  delete[] NorthWallTile;
  delete[] EastWallTile;
  delete[] SouthWallTile;
  delete[] WestWallTile;

  delete[] NorthDoorTile;
  delete[] EastDoorTile;
  delete[] SouthDoorTile;
  delete[] WestWallTile;  

  ReleaseRoom(EntryRoom);
}

void Level::ReleaseRoom(Room* room)
{
  if (room == NULL || room->FlaggedForDeletion)
  {
    return;
  }
  
  room->FlaggedForDeletion = true;
  ReleaseRoom(room->NorthNeighbor);
  ReleaseRoom(room->EastNeighbor);
  ReleaseRoom(room->SouthNeighbor);
  ReleaseRoom(room->WestNeighbor);

  delete room;
  room = NULL;
}

