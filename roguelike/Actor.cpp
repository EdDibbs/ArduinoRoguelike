#include "Actor.h"
#include "Screen.h"
#include "Level.h"

void Actor::Draw()
{
  for ( int x = 0; x < Width; x++)
  {
    for (int y = 0; y < Height; y++)
    {
      int index = (y * Width) + x;
      if (FlipHorizontalDraw)
      {
        x = Width - x - 1;
      }

      //add 2 for the width and height values
      int value = (int)pgm_read_word_near(CurSpritePtr + 2 + index);

      //because we're using all possible values of a uint16, we need to specify a
      //color that will denote transparency. In this case, it's the equivalent of
      //magenta. These values will be ignored (not drawn).
      if (value != 0xF81F)
      {
        Screen::Instance().DrawPixel(CurPosX + x, CurPosY + y, value);
      }
    }
  }
}

void Actor::Undraw()
{
  long startTime = millis();
  
  //create buffers
  uint16_t* fillBuffer = new uint16_t[LastWidth * LastHeight];  
  
  //find the bg tiles that could have been where we were  
  Room* curRoom = CurLevel->CurrentRoom;

  //get the types of tiles around us at the time
  //and put them into our buffer

  uint16_t* tileSprite = NULL;
  int lastOldXTile = -1;
  int lastOldYTile = -1;
  for (int x = LastPosX; x < LastPosX + LastWidth; x++)
  {
    for (int y = LastPosY; y < LastPosY + LastHeight; y++)
    {
      //preset our buffer to transparent
      int index = ((y - LastPosY) * LastWidth) + (x - LastPosX);
      fillBuffer[index] = 0xF81F;
      
      int oldXTile = (x - LevelDrawXOffset) / TileWidth;
      int oldYTile = (y - LevelDrawYOffset) / TileHeight;
      
      if (oldXTile >= LevelWidth || oldYTile >= LevelHeight) {
        continue;
      }

      //don't bother reloading the tile if we are doing the same tile
      if (oldXTile != lastOldXTile || oldYTile != lastOldYTile)
      {
        if (tileSprite != NULL) delete[] tileSprite;
        TileType tileType = curRoom->Tiles[(oldYTile * LevelWidth) + oldXTile];
        tileSprite = CurLevel->GetTilePixelsCopyByType(tileType);

        lastOldXTile = oldXTile;
        lastOldYTile = oldYTile;
      }
      
      //get the exact pixel on the tile that we were at
      int tileXpos = (x - LevelDrawXOffset) % TileWidth;
      int tileYpos = (y - LevelDrawYOffset) % TileHeight;     

      //store the pixel in our buffer
      fillBuffer[index] = tileSprite[(tileYpos * TileWidth) + tileXpos];
    }
  }
  if (tileSprite != NULL) 
  {
      delete[] tileSprite;
      tileSprite = NULL;
  }
  
  //TODO: get the actors in the spot where we were

  //fill in the hole of where we were  
  for ( int x = 0; x < LastWidth; x++)
  {
    for (int y = 0; y < LastHeight; y++)
    {
      int index = (y * LastWidth) + x;
      
      //find out where our old sprite was 
      //add 2 for the width and height values
      int value = (int)pgm_read_word_near(LastSpritePtr + 2 + index);

      //if the sprite was transparent at this spot, make the buffer transparent too.
      if (value == 0xF81F)
      {
        fillBuffer[index] = 0xF81F; //fill with transparency
      }
    }
  }
  
  Screen::Instance().Draw(LastPosX, LastPosY, LastWidth, LastHeight, fillBuffer);
  delete[] fillBuffer;

  long timeTaken = millis() - startTime;
  Serial.print("Undraw took ");
  Serial.print(timeTaken);
  Serial.println(" ms.");
}

void Actor::Move(float xDir, float yDir)
{
  int screenWidth = Screen::Instance().Width();
  int screenHeight = Screen::Instance().Height();

  LastPosX = CurPosX;
  CurPosX = CurPosX + (int)(MoveSpeed * xDir);
  if (CurPosX + Width > LevelMaxX) CurPosX = LevelMaxX - Width;
  if (CurPosX < LevelMinX) CurPosX = LevelMinX;

  LastPosY = CurPosY;
  CurPosY = CurPosY + (int)(MoveSpeed * yDir);
  if (CurPosY + Height > LevelMaxY) CurPosY = LevelMaxY - Height;
  if (CurPosY < LevelMinY) CurPosY = LevelMinY;

  //check against map for collisions

  //move back to where we were in case of collisions

  //move us in the playfield if necessary
  UpdatePlaygridLoc();

  UpdateMovement();
}
void Actor::UpdatePlaygridLoc()
{
  if (CurLevel != NULL && CurLevel->CurrentRoom != NULL)
  {
    //get our old grid coords
    int oldX = (LastPosX - LevelDrawXOffset) / TileWidth;
    int oldY = (LastPosY - LevelDrawYOffset) / TileHeight;

    //get our new grid coords
    int newX = (CurPosX - LevelDrawXOffset) / TileWidth;
    int newY = (CurPosY - LevelDrawYOffset) / TileHeight;

    if (newX != oldX || newY != oldY)
    {
//      Serial.print("Moving from [");
//      Serial.print(oldX);
//      Serial.print(", ");
//      Serial.print(oldY);
//      Serial.print("] to [");
//      Serial.print(newX);
//      Serial.print(", ");
//      Serial.print(newY);
//      Serial.println("]...");

      //take us out of the old grid
      if (CurLevel->CurrentRoom->cells != NULL && CurLevel->CurrentRoom->cells[oldX][oldY] != NULL)
      {
        Unit* unit = CurLevel->CurrentRoom->cells[oldX][oldY];

        //try to find us (a unit with this id
        while (unit != NULL && unit->actor != NULL && unit->actor->UniqueId != this->UniqueId)
        {
          unit = unit->next;
        }

        //check if we found ourselves
        if (unit != NULL && unit->actor != NULL && unit->actor->UniqueId == this->UniqueId)
        {
          //break the link to this actor
          if (unit->prev != NULL)
          {
            unit->prev->next = unit->next;
          }
          else
          {
            CurLevel->CurrentRoom->cells[oldX][oldY] = unit->next;
          }
          //free the memory
          delete unit;
        }
      }

      //put us in the new grid square
      if (CurLevel->CurrentRoom->cells != NULL)
      {
        Unit* newUnit = new Unit;
        newUnit->actor = this;
        newUnit->prev = NULL;
        newUnit->next = CurLevel->CurrentRoom->cells[newX][newY];

        CurLevel->CurrentRoom->cells[newX][newY] = newUnit;
      }

    }
  }
}

void Actor::UpdateMovement()
{
  uint8_t newDir = 255;
  bool needToDraw = false;
  if (CurPosX != LastPosX || CurPosY != LastPosY)
  {
    needToDraw = true;

    uint8_t xMovement, yMovement;

    //get the absolute difference between the current and last positions
    xMovement = CurPosX > LastPosX ? CurPosX - LastPosX : LastPosX - CurPosX;
    yMovement = CurPosY > LastPosY ? CurPosY - LastPosY : LastPosY - CurPosY;

    //did we more more in the xDirection?
    if (xMovement > yMovement)
    {
      //did we move left or right?
      newDir = CurPosX > LastPosX ? 1 : 3;
    }
    else
    {
      //did we move up or down?
      newDir = CurPosY > LastPosY ? 2 : 0;
    }
  }

  LastSpritePtr = CurSpritePtr;
  UpdateAnimationFrame(newDir);
  LastDir = newDir;

  LastWidth = Width;
  LastHeight = Height;
  Width = (short)pgm_read_word_near(CurSpritePtr);
  Height = (short)pgm_read_word_near(CurSpritePtr + 1);

  if (needToDraw)
  {
    Undraw();
    Draw();
  }
}
