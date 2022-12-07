#ifndef _LCD_H_
#define _LCD_H_
#include <Arduino.h>

void initDisplay();     // Initialize displays
void sendDisplay();     // Show data to displays
void printWeatherIcon(int id);  // Print weather icon on display
String get_dow();           // Returns day of week
String get_month();         // Returns month

static void bmpDrawCallbackS(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h); // Draw callback Square
static void bmpDrawCallbackR(int16_t x, int16_t y, uint16_t *bitmap, int16_t w, int16_t h); // Draw callback Round
void drawBmpS(const char *filename, int16_t x, int16_t y, int16_t xx, int16_t yy);  // Draw image on square display
void drawBmpR(const char *filename, int16_t x, int16_t y, int16_t xx, int16_t yy);  // Draw image on round display

#endif  // _LCD_H_