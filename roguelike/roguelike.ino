#include <Adafruit_GFX.h>   //Core graphics library
#include <Adafruit_ST7735.h> //hardware-specific library
#include <SPI.h> //??

// For the breakout, you can use any 2 or 3 pins
// These pins will also work for the 1.8" TFT shield
#define TFT_CS     10
#define TFT_RST    0  // you can also connect this to the Arduino reset
                      // in which case, set this #define pin to 0!
#define TFT_DC     8

// Option 1 (recommended): must use the hardware SPI pins
// (for UNO thats sclk = 13 and sid = 11) and pin 10 must be
// an output. This is much faster - also required if you want
// to use the microSD card (see the image drawing example)
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// Option 2: use any pins but a little slower!
#define TFT_SCLK 13   // set these to be whatever pins you like!
#define TFT_MOSI 11   // set these to be whatever pins you like!
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


int playerX;
int playerY;
int playerSpeed = 5;


//clock stuff
long startTime;
int count = 0;
int lastFPS = 0;

void setup() {  
   // Use this initializer if you're using a 1.8" TFT
  tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);

  playerX = tft.width() / 2;
  playerY = tft.height() / 2;

  startTime = millis();
  //setup serial communication
  //Serial.begin(9600);
}

void movePlayer()
{
  float UpDown = ((analogRead(A7) / 1020.0) - 0.5) * -1;
  float LeftRight = ((analogRead(A6) / 1020.0) - 0.5 ) * -1;
  
  playerX = playerX + (int)(playerSpeed * LeftRight);
  if (playerX > tft.width()) playerX = tft.width();
  if (playerX < 0) playerX = 0;
  
  playerY = playerY + (int)(playerSpeed * UpDown);
  if (playerY > tft.height()) playerY = tft.height();
  if (playerY < 0) playerY = 0;
}

void printFPS(int fps, uint64_t color)
{
  tft.setCursor(1,1);
  tft.setTextColor(color);
  tft.setTextSize(2);
  tft.print(fps, DEC);
}

void loop() {  
  printFPS(lastFPS, ST7735_WHITE);
  
  count++;
  if (millis() - startTime >= 1000)
  {
    printFPS(lastFPS, ST7735_BLACK);
    lastFPS = count;
    
    startTime = millis();
    count = 0;
  }
  
  tft.drawPixel(playerX, playerY, ST7735_BLACK);
  movePlayer();   
 
  tft.drawPixel(playerX, playerY, ST7735_WHITE);
  //delay(10);  
}
