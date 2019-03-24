#include "ili9486.h"
#include <wiringPi.h>
#include <math.h>
#include <stdlib.h>

int _random(int max){
	return rand() % max + 1;
}

int main() {
    initScreen(LANDSCAPE);

  int buf[478];
  int x, x2;
  int y, y2;
  int r, i;

// Clear the screen and draw the frame
  clearScreen();

  fillRect(Point(0, 0), Point(479, 13), COLOR_RED);
  fillRect(Point(0, 306), Point(479, 319), Color(64, 64, 64));
  printStr("* Universal Color TFT Display Library *", Point(CENTER, 1), SMALL, 0, COLOR_WHITE, COLOR_RED);
  printStr("<http://www.RinkyDinkElectronics.com/>", Point(CENTER, 307), SMALL, 0, COLOR_YELLOW, Color(64, 64, 64));

  drawRect(Point(0, 14), Point(479, 305), COLOR_BLUE);

// Draw crosshairs
  drawLine(Point(239, 15), Point(239, 304), COLOR_BLUE);
  drawLine(Point(1, 159), Point(478, 159), COLOR_BLUE);
  for (i=9; i<470; i+=10)
    drawLine(Point(i, 157), Point(i, 161), COLOR_BLUE);
  for (i=19; i<220; i+=10)
    drawLine(Point(237, i), Point(241, i), COLOR_BLUE);

// Draw sin-, cos- and tan-lines
  printStr("Sin", Point(5, 15), SMALL, 0, COLOR_CYAN, COLOR_BLACK);
  for (i=1; i<478; i++)
    drawPixel(Point(i, 159+(sin(((i*1.13)*3.14)/180)*95)), COLOR_CYAN);

  printStr("Cos", Point(5, 27), SMALL, 0, COLOR_RED, COLOR_BLACK);
  for (int i=1; i<478; i++)
    drawPixel(Point(i,159+(cos(((i*1.13) * 3.14) / 180) * 95)), COLOR_RED);

  printStr("Tan", Point(5, 39), SMALL, 0, COLOR_YELLOW, COLOR_BLACK);
  for (int i=1; i<478; i++)
    drawPixel(Point(i,159+(tan(((i*1.13)*3.14)/180))), COLOR_YELLOW);

  delay(2000);

  fillRect(Point(1,15),Point(478,304), COLOR_BLACK);
  drawLine(Point(239, 15), Point(239, 304), COLOR_BLUE);
  drawLine(Point(1, 159), Point(478, 159), COLOR_BLUE);

// Draw a moving sinewave
  x=1;
  for (i=1; i<(478*15); i++)
  {
    x++;
    if (x==479)
      x=1;
    if (i>479)
    {
      if ((x==239)||(buf[x-1]==159))
        drawRect(Point(0, 14), Point(479, 305), COLOR_BLUE);
      drawPixel(Point(x,buf[x-1]), COLOR_BLACK);
    }
    y=159+(sin(((i*0.7)*3.14)/180)*(90-(i / 100)));
    drawPixel(Point(x,y), COLOR_CYAN);
    buf[x-1]=y;
  }

  delay(2000);

  fillRect(Point(1,15),Point(478,304), COLOR_BLACK);

// Draw some filled rectangles
  for (i=1; i<6; i++)
  {
	RGB* color = NULL;
    switch (i)
    {
      case 1:
	color = &COLOR_MAGENTA;
        break;
      case 2:
        color = &COLOR_RED;
        break;
      case 3:
	color = &COLOR_GREEN;
        break;
      case 4:
	color = &COLOR_BLUE;
        break;
      case 5:
	color = &COLOR_YELLOW;
        break;
    }
    fillRect(Point(150+(i*20), 70+(i*20)), Point(210+(i*20), 130+(i*20)), *color);
  }

  delay(2000);
  
  fillRect(Point(1,15),Point(478,304), COLOR_BLACK);

// Draw some filled, rounded rectangles
  for (int i=1; i<6; i++)
  {
    RGB* color = NULL;
    switch (i)
    {
      case 1:
        color = &COLOR_MAGENTA;
        break;
      case 2:
	color = &COLOR_RED;
        break;
      case 3:
	color = &COLOR_GREEN;
        break;
      case 4:
	color = &COLOR_BLUE;
        break;
      case 5:
	color = &COLOR_YELLOW;
        break;
    }
    fillRoundRect(Point(270-(i*20), 70+(i*20)), Point(330-(i*20), 130+(i*20)), *color);
  }
  
  delay(2000);
  
  fillRect(Point(1,15), Point(478,304), COLOR_BLACK);

// Draw some filled circles
  for (int i=1; i<6; i++)
  {
    RGB* color = NULL;
    switch (i)
    {
      case 1:
	color = &COLOR_MAGENTA;
        break;
      case 2:
	color = &COLOR_RED;
        break;
      case 3:
	color = &COLOR_GREEN;
        break;
      case 4:
	color = &COLOR_BLUE;
        break;
      case 5:
	color = &COLOR_YELLOW;
        break;
    }
    fillCircle(Point(180+(i*20),100+(i*20)), 30, *color);
  }
  delay(2000);
  fillRect(Point(1,15),Point(478,304), COLOR_BLACK);

// Draw some lines in a pattern
  for (int i=15; i<304; i+=5)
    drawLine(Point(1, i), Point((i*1.6)-10, 304), COLOR_RED);

  for (int i=304; i>15; i-=5)
    drawLine(Point(478, i), Point((i*1.6)-11, 15), COLOR_RED);

  for (int i=304; i>15; i-=5)
    drawLine(Point(1, i), Point(491-(i*1.6), 15), COLOR_CYAN);

  for (int i=15; i<304; i+=5)
    drawLine(Point(478, i), Point(490-(i*1.6), 304), COLOR_CYAN);

  delay(2000);

  fillRect(Point(1,15), Point(478,304), COLOR_BLACK);

// Draw some random circles
  for (i=0; i<100; i++)
  {
    RGB color = Color(_random(255), _random(255), _random(255));
    x=32+_random(416);
    y=45+_random(226);
    r=_random(30);
    drawCircle(Point(x, y), r, color);
  }

  delay(2000);
  
  fillRect(Point(1,15),Point(478,304), COLOR_BLACK);

// Draw some random rectangles
  for (i=0; i<100; i++)
  {
    RGB color = Color(_random(255), _random(255), _random(255));
    x=2+_random(476);
    y=16+_random(289);
    x2=2+_random(476);
    y2=16+_random(289);
    drawRect(Point(x, y), Point(x2, y2), color);
  }

  delay(2000);

  fillRect(Point(1,15),Point(478,304), COLOR_BLACK);

// Draw some random rounded rectangles
  for (i=0; i<100; i++)
  {
    RGB color = Color(_random(255), _random(255), _random(255));
    x=2+_random(476);
    y=16+_random(289);
    x2=2+_random(476);
    y2=16+_random(289);
    drawRoundRect(Point(x, y), Point(x2, y2), color);
  }

  delay(2000);

  fillRect(Point(1,15), Point(478,304), COLOR_BLACK);

  for (i=0; i<100; i++)
  {
    RGB color = Color(_random(255), _random(255), _random(255));
    x=2+_random(476);
    y=16+_random(289);
    x2=2+_random(476);
    y2=16+_random(289);
    drawLine(Point(x, y), Point(x2, y2), color);
  }

  delay(2000);
  
  fillRect(Point(1,15),Point(478,304), COLOR_BLACK);

  for (i=0; i<10000; i++)
  {
    RGB color = Color(_random(255), _random(255), _random(255));
    drawPixel(Point(2+_random(476), 16+_random(289)), color);
  }

  delay(2000);

  fillScreen(COLOR_BLUE);
  fillRoundRect(Point(160, 70), Point(319, 169), COLOR_RED);
  
  printStr("That's it!", Point(CENTER, 93), SMALL, 0, COLOR_WHITE, COLOR_RED);
  printStr("Restarting in a", Point(CENTER, 119), SMALL, 0, COLOR_WHITE, COLOR_RED);
  printStr("few seconds...", Point(CENTER, 132), SMALL, 0, COLOR_WHITE, COLOR_RED);
  
  printStr("Runtime: (msecs)", Point(CENTER, 290), SMALL, 0, COLOR_GREEN, COLOR_BLUE);

  usleep (10000);

  return 0;
}
