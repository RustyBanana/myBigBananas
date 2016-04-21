//By William Chen
//Date 21/04/2016
//returns color of pixels depending on steps to escape

#include "pixelColor.h"

#define MAX_STEPS 255
#define MAX_RED 255
#define MAX_BLUE 255
#define MAX_GREEN 255

static double getPercent(int steps);

unsigned char stepsToRed (int steps) {
   unsigned char intensity;
   if (steps < 255) {
      intensity =  getPercent(steps) * MAX_RED;
   } else {
      intensity = 0;
   }
   return intensity;
}

unsigned char stepsToBlue (int steps) {
   unsigned char intensity;
   if (steps < 255) {
      intensity =  getPercent(steps) * MAX_BLUE;
   } else {
      intensity = 0;
   }
   return intensity;
}

unsigned char stepsToGreen (int steps) {
   unsigned char intensity;
   if (steps < 255) {
      intensity =  getPercent(steps) * MAX_GREEN;
   } else {
      intensity = 0;
   }
   return intensity;
}

static double getPercent(int steps) {
   double percent = (steps* 1.0)/MAX_STEPS;
   return percent;
}
