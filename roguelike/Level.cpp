#define NULL 0
#include "Macros.h"
#include "Level.h"
#include "Room.h"
#include "Screen.h"
#include "SpriteDefinitions.h"
#include "MemoryFree.h"
#include "MobBat.h"

Level::Level(LevelType type)
{
  Sprintln(F("Loading level..."));
  long startTime = millis();
  
  switch (type)
  {
    case Jungle:  LoadJungleSprites();  break;
    case Stone:   LoadStoneSprites();   break;
  }

  long timeTaken = millis() - startTime;

  Sprint(F("Took "));
  Sprint(timeTaken);
  Sprintln(F(" ms to load level."));

  //entry
  CurrentRoom = GenerateTestRoom();

  //room 2
  CurrentRoom->NorthNeighbor = GenerateTestRoom();
  CurrentRoom->NorthNeighbor->SouthNeighbor = CurrentRoom;

  //room 3
  CurrentRoom->NorthNeighbor->EastNeighbor = GenerateTestRoom();
  CurrentRoom->NorthNeighbor->EastNeighbor->WestNeighbor = CurrentRoom->NorthNeighbor;
  
  DrawRoom();  
}

Level::~Level()
{
  delete[] FloorTile;
  delete[] CornerTile;
  delete[] NorthSouthWallTile;  
  delete[] WestEastWallTile;
  delete[] NorthSouthDoorTile;
  delete[] WestEastWallTile;  

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

void Level::PopulateRoomWithMobs()
{
  //add some bats
  for (int batCount = 0; batCount < 3; batCount++)
  {
    MobBat* bat = new MobBat(this);
    bat->SetPosition(25 + (batCount * 3), LevelMaxY - TileHeight - bat->Height - (batCount * (bat->Height + 10)));
  }
  
}

void Level::UnlockDoors()
{
    
}

uint16_t* Level::GetTilePixelsCopyByType(TileType type)
{
  byte flip = 0;
  byte rot = 0;
  uint16_t* sprite = NULL;
  
  switch (type)
      {
          case WallNorth:   sprite = NorthSouthWallTile;                   break;
          case WallEast:    sprite = WestEastWallTile;   flip = FlipHori;  break;
          case WallSouth:   sprite = NorthSouthWallTile; flip = FlipVert;  break;
          case WallWest:    sprite = WestEastWallTile;                     break;
          case DoorNorth:   sprite = NorthSouthDoorTile;                   break;
          case DoorEast:    sprite = WestEastDoorTile;   flip = FlipHori;  break;
          case DoorSouth:   sprite = NorthSouthDoorTile; flip = FlipVert;  break;
          case DoorWest:    sprite = WestEastDoorTile;                     break;
          case FloorNormal: sprite = FloorTile;                            break;
          case FloorAlt:                                                   break;
          case FloorHole:                                                  break;
          case FloorRock:                                                  break;
          case CornerNW:    sprite = CornerTile;                           break;
          case CornerNE:    sprite = CornerTile; rot = Rot90;              break;          
          case CornerSE:    sprite = CornerTile; rot = Rot180;             break;
          case CornerSW:    sprite = CornerTile; rot = Rot270;             break;
      }

  if (sprite == NULL)
  {
      Sprint(F("Couldn't get tile pixels by type: "));
      Sprintln(type);
      sprite = FloorTile;
  }

  return Screen::Instance().FlipAndRotateImage(TileWidth, TileHeight, sprite, flip, rot);
}

void Level::LoadJungleSprites()
{
  for(int i = 0; i < 6; i++)
  {
    const uint16_t* tileHeadMemory;
    uint16_t* tileHeadLocal = new uint16_t[TileWidth * TileHeight];
    switch (i)
    {
      case 0: tileHeadMemory = JUNGLE_FLOOR; break;

      //corners
      case 1: tileHeadMemory = JUNGLE_WALL_CORNER; break;
      
      //N/S
      case 2: tileHeadMemory = JUNGLE_NWALL_1; break;

      //E/W
      case 3: tileHeadMemory = JUNGLE_WWALL_1; break;      

      //N/S door
      case 4: tileHeadMemory = JUNGLE_NWALL_DOOR; break;

      //E/W door
      case 5: tileHeadMemory = JUNGLE_WWALL_DOOR; break;
    }

    //copy the tile from flash mem to local RAM
    for (int row = 0; row < TileWidth; row++)
    {      
      for (int col = 0; col < TileHeight; col++)
      {
        int index = (col * TileWidth) + row;
        
        tileHeadLocal[index] = pgm_read_word_near(tileHeadMemory + 2 + index);
      }
    }

    //save the local copy to the correct tile
    switch (i)
    {
      case 0: FloorTile     = tileHeadLocal;  break;
      case 1: CornerTile = tileHeadLocal; break;
      case 2: NorthSouthWallTile = tileHeadLocal;  break;      
      case 3: WestEastWallTile  = tileHeadLocal;  break;      
      case 4: NorthSouthDoorTile = tileHeadLocal;  break;
      case 5: WestEastDoorTile  = tileHeadLocal;  break;      
    }

    Sprint(F("Loaded "));
    Sprint(i);
    Sprint(F(" tiles. "));
    Sprint(freeMemory());
    Sprintln(F(" bytes of memory remaining."));
  }
}

void Level::LoadStoneSprites()
{
  for(int i = 0; i < 5; i++) //UPDATE THIS TO 6 WHEN WE HAVE THE WEST DOOR TILE
  {
    const uint16_t* tileHeadMemory;
    uint16_t* tileHeadLocal = new uint16_t[TileWidth * TileHeight];
    switch (i)
    {
      case 0: tileHeadMemory = STONE_FLOOR; break;

      //corners
      case 1: tileHeadMemory = STONE_NECORNER; break;
      
      //N/S
      case 2: tileHeadMemory = STONE_NWALL; break;

      //E/W
      case 3: tileHeadMemory = STONE_WWALL; break;      

      //N/S door
      case 4: tileHeadMemory = STONE_NWALL_DOOR; break;

      //E/W door
      case 5: tileHeadMemory = STONE_EWALL_DOOR;  break;
    }

    //copy the tile from flash mem to local RAM
    for (int row = 0; row < TileWidth; row++)
    {      
      for (int col = 0; col < TileHeight; col++)
      {
        int index = (col * TileWidth) + row;
        
        tileHeadLocal[index] = pgm_read_word_near(tileHeadMemory + 2 + index);
      }
    }

    //save the local copy to the correct tile
    switch (i)
    {
      case 0: FloorTile     = tileHeadLocal;  break;
      case 1: CornerTile = tileHeadLocal; break;
      case 2: NorthSouthWallTile = tileHeadLocal;  break;      
      case 3: WestEastWallTile  = tileHeadLocal;  break;      
      case 4: NorthSouthDoorTile = tileHeadLocal;  break;
      case 5: WestEastDoorTile  = tileHeadLocal;  break;      
    }

    Sprint(F("Loaded "));
    Sprint(i);
    Sprint(F(" tiles. "));
    Sprint(freeMemory());
    Sprintln(F(" bytes of memory remaining."));
  }
}

Room* Level::GenerateTestRoom()
{
  Room* room = new Room();
  room->Tiles = new TileType[LevelWidth*LevelHeight];
  for (int x = 0; x < LevelWidth; x++)
    {    
      for (int y = 0; y < LevelHeight; y++)
      {
        room->cells[x][y] = NULL;        
      }
    }
    
  for (int col = 0; col < LevelWidth; col++)
  {
    for (int row = 0; row < LevelHeight; row++)
    {
      TileType type;
      if (row == 0 && col == 0) type = CornerNW;
      else if (row == 0 && col == 12) type = CornerNE;
      else if (row == 7 && col == 0) type = CornerSW;
      else if (row == 7 && col == 12) type = CornerSE;
      else if (row == 0 && col == 6) type = DoorNorth;
      else if (row == 0) type = WallNorth;
      else if (row == 7 && col == 6) type = DoorSouth;
      else if (row == 7) type = WallSouth;
      else if (col == 0 && row == 4) type = DoorWest;
      else if (col == 0) type = WallWest;
      else if (col == 12 && row == 4) type = DoorEast;
      else if (col == 12) type = WallEast;
      else type = FloorNormal;
      
      room->Tiles[row * LevelWidth + col] = type;
    }
  }

    return room;
}

void Level::DrawMap()
{
  //clear the current map
  Screen::Instance().DrawRect(48,0, 130 - 48, LevelDrawYOffset, 0x0000);
  
  ClearMapVisited(CurrentRoom);
  DrawMapRec(CurrentRoom, MapStartX, MapStartY);  
}

void Level::ClearMapVisited(Room* room)
{
  if (room == NULL || !room->DrawVisited) return;
  room->DrawVisited = false;

  ClearMapVisited(room->NorthNeighbor);
  ClearMapVisited(room->EastNeighbor);
  ClearMapVisited(room->SouthNeighbor);
  ClearMapVisited(room->WestNeighbor);
}

void Level::DrawMapRec(Room* room, int x, int y)
{
  Screen* screen = &Screen::Instance();
  if (room->DrawVisited || !room->PlayerVisited) return;

  room->DrawVisited = true;

  //draw the 'hallways'
  if (room->NorthNeighbor != NULL)
    screen->DrawRect((x+ MapRoomWidth/2) - (MapHallWidth/2), y - MapRoomYSpace, MapHallWidth, MapRoomYSpace, MapHallColor);
  if (room->EastNeighbor != NULL)
    screen->DrawRect((x + MapRoomWidth), (y + MapRoomHeight/2) - (MapHallWidth/2), MapRoomXSpace, MapHallWidth, MapHallColor);
  if (room->SouthNeighbor != NULL)
    screen->DrawRect((x + MapRoomWidth/2) - (MapHallWidth/2), (y + MapRoomHeight), MapHallWidth, MapRoomYSpace, MapHallColor);
  if (room->WestNeighbor != NULL)
    screen->DrawRect(x - MapRoomXSpace, (y+ MapRoomHeight/2) - (MapHallWidth/2), MapRoomXSpace, MapHallWidth, MapHallColor);


  //draw the room itself
  if (room == CurrentRoom)
    screen->DrawRect(x, y, MapRoomWidth, MapRoomHeight, MapCurRoomColor);
  else
    screen->DrawRect(x, y, MapRoomWidth, MapRoomHeight, MapRoomColor);

  
  //draw the neighbors
  if (room->NorthNeighbor != NULL)
    DrawMapRec(room->NorthNeighbor, x, y - MapRoomYSpace - MapRoomHeight);
  if (room->EastNeighbor != NULL)
    DrawMapRec(room->EastNeighbor, x + MapRoomXSpace + MapRoomWidth, y);
  if (room->SouthNeighbor != NULL)
    DrawMapRec(room->SouthNeighbor, x, y + MapRoomYSpace + MapRoomHeight);
  if (room->WestNeighbor != NULL)
    DrawMapRec(room->WestNeighbor, x - MapRoomXSpace - MapRoomWidth, y);
}

void Level::DrawRoom()
{
  long startTime = millis();
  
  for (int col = 0; col < LevelWidth; col++)
  {
    for (int row = 0; row < LevelHeight; row++)
    {
      TileType curTile = CurrentRoom->Tiles[row * LevelWidth + col];
      uint16_t* floorSprite;
      byte rot = 0;
      byte flip = 0;
       
      switch (curTile)
      {
          case WallNorth:   floorSprite = NorthSouthWallTile;                   break;
          case WallEast:    floorSprite = WestEastWallTile;   flip = FlipHori;  break;
          case WallSouth:   floorSprite = NorthSouthWallTile; flip = FlipVert;  break;
          case WallWest:    floorSprite = WestEastWallTile;                     break;
          case DoorNorth:   floorSprite = CurrentRoom->NorthNeighbor == NULL ? NorthSouthWallTile : NorthSouthDoorTile;                   break;
          case DoorEast:    floorSprite = CurrentRoom->EastNeighbor  == NULL ? WestEastWallTile : WestEastDoorTile;   flip = FlipHori;  break;
          case DoorSouth:   floorSprite = CurrentRoom->SouthNeighbor == NULL ? NorthSouthWallTile : NorthSouthDoorTile; flip = FlipVert;  break;
          case DoorWest:    floorSprite = CurrentRoom->WestNeighbor == NULL ? WestEastWallTile: WestEastDoorTile;                     break;
          case FloorNormal: floorSprite = FloorTile;                            break;
          case FloorAlt:
          case FloorHole:
          case FloorRock:
          case CornerNW:    floorSprite = CornerTile;                           break;
          case CornerNE:    floorSprite = CornerTile; rot = Rot90;              break;          
          case CornerSE:    floorSprite = CornerTile; rot = Rot180;             break;
          case CornerSW:    floorSprite = CornerTile; rot = Rot270;             break;
      }

      Screen::Instance().Draw(LevelDrawXOffset + (col * TileWidth), LevelDrawYOffset + (row * TileHeight), TileWidth, TileHeight, floorSprite, flip, rot);
    }
  }
  long totTime = millis() - startTime;

  Sprint(F("Took "));
  Sprint(totTime);
  Sprintln(F(" ms to draw the room."));

  //if we drew the room, that means we have visited this room, also we should draw the map
  CurrentRoom->PlayerVisited = true;
  DrawMap();
}

