#include "Actor.h"
#include "Screen.h"
#include "Level.h"

Actor::Actor(Level* curLevel)
{
  CurLevel = curLevel;
  UniqueId = GetUniqueId();
  LastPosX = CurPosX = SCREEN_WIDTH / 2;
  LastPosY = CurPosY = SCREEN_HEIGHT /2;
  LastWidth= Width = 0;
  LastHeight= Height = 0;  
}

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


  //This next loop nest goes through all pixels that we occupied during
  //our last frame, finds the corresponding pixel in the sprite of
  //the background tile that we stood in front of during our last frame.
  //It then writes this pixel to the fillBuffer.

  //So, we know that we occupied up to LastWidth amount of pixels on the horizontal
  //during our last draw. But we want to figure out how many of those pixels
  //were in the first of the four BG tiles that we're going to check.
  //We do this so that we only have to cache the BG tiles once each.

  uint8_t numXPixelsInQ1 = TileWidth  - ((LastPosX - LevelDrawXOffset) % TileWidth);
  uint8_t numYPixelsInQ1 = TileHeight - ((LastPosY - LevelDrawYOffset) % TileHeight);
//  Serial.print("LastPosX: ");
//  Serial.print(LastPosX);
//  Serial.print(", LevelDrawXOffset: ");
//  Serial.print(LevelDrawXOffset);
//  Serial.print(", TileWidth: ");
//  Serial.print(TileWidth);
//  Serial.print(", numXPixelsInQ1: ");
//  Serial.print(numXPixelsInQ1);
//  Serial.print(", NumYPixelsInQ1: ");
//  Serial.println(numYPixelsInQ1);

  int loadTileCount = 0;
  uint16_t* tileSprite = NULL;
  int lastOldXTile = -1;
  int lastOldYTile = -1;
  for (int quadrant = 1; quadrant < 5; quadrant++)
  {
    int minX, maxX, minY, maxY;
    switch (quadrant)
    {
      case 1:
        minX = LastPosX;        
        maxX = minX + numXPixelsInQ1;
        minY = LastPosY;
        maxY = minY + numYPixelsInQ1;
        break;
      case 2:
        minX = LastPosX + numXPixelsInQ1;        
        maxX = LastPosX + LastWidth;
        minY = LastPosY;
        maxY = minY + numYPixelsInQ1;
        break;
      case 3:
        minX = LastPosX;        
        maxX = minX + numXPixelsInQ1;
        minY = LastPosY + numYPixelsInQ1;
        maxY = LastPosY + LastHeight;
        break;
      case 4:
        minX = LastPosX + numXPixelsInQ1;        
        maxX = LastPosX + LastWidth;
        minY = LastPosY + numYPixelsInQ1;
        maxY = LastPosY + LastHeight;      
        break;
    }
//      Serial.print("quadrant: ");
//      Serial.print(quadrant);
//      Serial.print(", minX: ");
//      Serial.print(minX);
//      Serial.print(", maxX: ");
//      Serial.print(maxX);
//      Serial.print(", minY: ");
//      Serial.print(minY);
//      Serial.print(", maxY: ");
//      Serial.println(maxY);
    for (int x = minX; x < maxX && x < LastPosX + LastWidth; x++)
    {
      for (int y = minY; y < maxY && y < LastPosY + LastHeight; y++)
      {
        //preset our buffer to transparent
        int index = ((y - LastPosY) * LastWidth) + (x - LastPosX);
        fillBuffer[index] = 0xF81F;    

        //make sure we're not outside the scope of the actor's old window
        if (x >= LastPosX + LastWidth || y >= LastPosY + LastHeight)
        {
          continue;
        }        
        
        int oldXTile = (x - LevelDrawXOffset) / TileWidth;
        int oldYTile = (y - LevelDrawYOffset) / TileHeight;

        //account for if the sprite goes off the playfield
        if (oldXTile >= LevelWidth || oldXTile < 0
            || oldYTile >= LevelHeight || oldYTile < 0) {
          continue;
        }
  
        //don't bother reloading the tile if we are doing the same tile
        if (oldXTile != lastOldXTile || oldYTile != lastOldYTile)
        {
          if (tileSprite != NULL) delete[] tileSprite;
          TileType tileType = curRoom->Tiles[(oldYTile * LevelWidth) + oldXTile];
          tileSprite = CurLevel->GetTilePixelsCopyByType(tileType);
          loadTileCount++;
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
  }
  
  if (tileSprite != NULL) 
  {
      delete[] tileSprite;
      tileSprite = NULL;
  }
  
  short oldTileX = (LastPosX - LevelDrawXOffset) / TileWidth;
  short oldTileY = (LastPosY - LevelDrawYOffset) / TileHeight;
  
  for (int tileX = oldTileX - 2; tileX < oldTileX + 2; tileX++)
    {    
      if (tileX < 0 || tileX >= LevelWidth) continue;
      for (int tileY = oldTileY - 2; tileY < oldTileY + 2; tileY++)
      {        
        if (tileY < 0 || tileY >= LevelHeight) continue;
        
        //check all units, see if we collided with this unit
        Unit* units = CurLevel->CurrentRoom->cells[tileX][tileY];
        while (units != NULL)
        {          
          Actor* actor = units->actor;
          if (actor->UniqueId == UniqueId)
          {
            units = units->next;            
            continue;
          }

          //see if we collided with this actor
          if (LastPosX + LastWidth > actor->CurPosX
              && LastPosX < actor->CurPosX + actor->Width
              && LastPosY + LastHeight > actor->CurPosY
              && LastPosY < actor->CurPosY + actor->Height)
          {                     
          }
          else
          {
            units = units->next;
            continue;
          }
                    
          int count = 0;
          //loop through our pixels
          for ( int x = 0; x < LastWidth; x++)
          {
            for (int y = 0; y < LastHeight; y++)
            {              
              //calculate what pixel this is overlapped with the other actor
              int otherX = LastPosX + x - actor->CurPosX;
              int otherY = LastPosY + y - actor->CurPosY;

              if (otherX < 0 || otherX > actor->Width || otherY < 0 || otherY > actor->Height)
              {
                //this part of the actor didn't overlap with us
                continue;
              }
              
              count = count + 1;
              int index = (y * LastWidth) + x;
              int otherIndex = (otherY * actor->Width) + otherX;

              //check if the other actor had pixels in this spot, if not draw the ground we loaded before.
              uint16_t value = (uint16_t)pgm_read_word_near(actor->CurSpritePtr + 2 + otherIndex);
              if (value != 0xF81F)
              {
                fillBuffer[index] = value;
              }
            }            
          }

          units = units->next;
        }        
      }
    }
    
  //fill in the hole of where we were  
  for ( int x = 0; x < LastWidth; x++)
  {
    for (int y = 0; y < LastHeight; y++)
    {
      int index = (y * LastWidth) + x;
      
      //find out where our old sprite was 
      //add 2 for the width and height values
      uint16_t value = (uint16_t)pgm_read_word_near(LastSpritePtr + 2 + index);

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
//  Serial.print(F("Undraw took "));
//  Serial.print(timeTaken);
//  Serial.println(F(" ms."));
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

        //try to find us (a unit with this id)
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
  bool weMoved = false;
  if (CurPosX != LastPosX || CurPosY != LastPosY)
  {
    weMoved = true;

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

  if (weMoved || CurSpritePtr != LastSpritePtr)
  {
    Undraw();
    Draw();
  }
}


void Actor::SetPosition(int xpos, int ypos)
{
  LastPosX = CurPosX;
  LastPosY = CurPosY;
  
  CurPosX = xpos;
  CurPosY = ypos;
  
  //UpdatePlaygridLoc();
  UpdateMovement();
}
