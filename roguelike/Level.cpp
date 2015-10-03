#define NULL 0
#include "Level.h"
#include "Screen.h"
#include "SpriteDefinitions.h"
#include "MemoryFree.h"

Level::Level(LevelType type)
{
  Serial.println("Loading level...");
  long startTime = millis();
  
  switch (type)
  {
    case Jungle:
    {
      LoadJungleSprites();     
    }
    break;
  }

  long timeTaken = millis() - startTime;

  Serial.print("Took ");
  Serial.print(timeTaken);
  Serial.println(" ms to load level.");

  TestDraw();
}

Level::~Level()
{
  delete[] FloorTile;
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

void Level::LoadJungleSprites()
{
  for(int i = 0; i < 9; i++)
  {
    const uint16_t* tileHeadMemory;
    uint16_t* tileHeadLocal = new uint16_t[TileWidth * TileHeight];
    bool flipHori = false;
    bool flipVert = false;
    switch (i)
    {
      case 0: tileHeadMemory = JUNGLE_FLOOR_TILE; break;

      //N/S
      case 1: tileHeadMemory = JUNGLE_NWALL1; break;
      case 2: tileHeadMemory = JUNGLE_NWALL1; break;

      //E/W
      case 3: tileHeadMemory = JUNGLE_WWALL1; flipHori = true; break;
      case 4: tileHeadMemory = JUNGLE_WWALL1; break;

      //N/S door
      case 5: tileHeadMemory = JUNGLE_NWALL_DOOR; break;
      case 6: tileHeadMemory = JUNGLE_NWALL_DOOR; flipVert = true; break;

      //E/W door
      case 7:
      case 8: //MISSING THIS SPRITE 
              break;
    }

    //copy the tile from flash mem to local RAM
    for (int row = 0; row < TileWidth; row++)
    {      
      for (int col = 0; col < TileHeight; col++)
      {
        int index;
        if (flipHori && flipVert) index = ((TileHeight - col - 1) * TileWidth)    + (TileWidth - row - 1);
        else if (flipHori)        index = (col * TileWidth)                       + (TileWidth - row - 1);
        else if (flipVert)        index = ((TileHeight - col - 1) * TileWidth)    + row;
        else                      index = (col * TileWidth)                       + row;
        
        tileHeadLocal[(col * TileWidth) + row] = pgm_read_word_near(tileHeadMemory + 2 + index);
      }
    }

    //save the local copy to the correct tile
    switch (i)
    {
      case 0: FloorTile     = tileHeadLocal;  break;
      case 1: NorthWallTile = tileHeadLocal;  break;
      case 2: SouthWallTile = tileHeadLocal;  break;
      case 3: EastWallTile  = tileHeadLocal;  break;      
      case 4: WestWallTile  = tileHeadLocal;  break;
      case 5: NorthDoorTile = tileHeadLocal;  break;
      case 6: SouthDoorTile = tileHeadLocal;  break;
      case 7: EastDoorTile  = tileHeadLocal;  break;      
      case 8: WestDoorTile  = tileHeadLocal;  break;
    }

    Serial.print("Loaded ");
    Serial.print(i);
    Serial.print(" tiles. ");
    Serial.print(freeMemory());
    Serial.println(" bytes of memory remaining.");
  }
}

void Level::TestDraw()
{
  int yOffset = 24;
  for (int col = 0; col < 13; col++)
  {
    for (int row = 0; row < 8; row++)
    {
      uint16_t* pixels;
      if (row == 0) pixels = NorthWallTile;
      else if (row == 7) pixels = SouthWallTile;
      else if (col == 0) pixels = WestWallTile;
      else if (col == 12) pixels = EastWallTile;
      else pixels = FloorTile;
      



      Screen::Instance().Draw(col * TileWidth, (row * TileHeight) + yOffset, TileWidth, TileHeight, pixels);
    }
  }
}

