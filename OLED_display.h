#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <PMW3360.h>
#include "pixel_data.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin) // change to -1 for 128x64 display
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; Address is the same for 128x64 display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define MAIN_MENU 0
#define LINE_MENU 1
#define WIDTH_MENU 2
#define SETTINGS_MENU 3
#define JITTER_MENU 4
#define DROPMENU 5

void mainmenu();
void linemenu();
void widthmenu();
void jittermenu();
void settingsmenu();
void dropmenu();

void string(String text, int x, int y);
void highlight_string(String text, int x, int y);
void refresh_display();
void back();
void disp_printing();
void disp_cancelled();

//Button Pins
const int scroll_button = 18; //pressing both buttons when not printing = back function
const int select_button = 19; //pressing both buttons when printing = cancels printing

int menu_id = 0;
int n_fire = 1; //number of times to fire in one position
boolean comp_jitter = false; //bits shifted to compensate for hand jitter when true

boolean printer_on = false;
boolean printImage = false; 
boolean printLine = false;
boolean powerSwitch = false; 

//stores pixel data for the last chosen line width. Initialized to width 2 (16 nozzles) by default.
byte lineData[16] = {0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b11111111,0b11111111,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000,0b00000000}; 

void back();
boolean back_ = false;

//counters to keep track of scrolling postion
int main_count = 1;
int printline_count = 1;
int width_count = 1;
int settings_count = 1;
int jittering_count = 1;
int drop_count = 1;


// Back Arrow, 128x64px
const unsigned char myBitmap [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void refresh_display() {
  //----------------------MAIN MENU------------------------------
  switch (menu_id)
  {
    case 0:
      mainmenu();
      break;
    case 1:
      linemenu();
      break;
    case 2:
      widthmenu();
      break;
    case 3:
      settingsmenu();
      break;
    case 4:
      jittermenu();
      break;
    case 5:
      dropmenu();
      break;
    default:
      break;
  }
}

//Functions:
void string(String text, int x, int y) {
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(x, y);      
  display.println(text);
}

void highlight_string(String text, int x, int y) {
  display.setTextSize(1);      
  display.setTextColor(BLACK, SSD1306_WHITE); 
  display.setCursor(x, y);     
  display.print(text);
}

void back() {
  display.clearDisplay();
  display.setTextSize(2);      
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(2, 10);     
  highlight_string(" OPTIONS ", 15, 0);
  display.drawBitmap(0, 0, myBitmap, 128, 64, WHITE);
  display.display();
  delay(1);
}

void disp_printing() {
  display.clearDisplay();
  display.setTextSize(2);      
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(2, 10);     
  string("Printing...", 15, 12);
  display.display();
  delay(1);
}

void disp_cancelled() {
  display.clearDisplay();
  display.setTextSize(2);     
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(2, 10);      
  string("CANCELLED Printing", 15, 12);
  display.display();
  delay(1);
}

void mainmenu(){
  if (back_ == true) {
    delay(200);
    back_ = false;
  }
  
  if(printer_on){    
    //--------------------------------------- Scroll Counter ----------------------------------------
    if (digitalRead(scroll_button) == LOW && digitalRead(select_button) == HIGH) {
      delay(300);
      main_count++;
      if (main_count > 4) {
        main_count = 1;
      }
    }
    
    //---------------------------------------- Print Line -------------------------------------------
    if (main_count == 1 && (digitalRead(select_button) == HIGH)) {
      display.clearDisplay();
      highlight_string(" PRINT-A-SKETCH ", 15, 0);
      string("Print Line", 4, 12);
      string("Print Image", 4, 24);
      string("Settings", 4, 36);
      string("Power: ON", 4, 48);
      display.drawRect(1, 10, 127, 11, WHITE);
      display.display();
      delay(1);
    }
    if ((digitalRead(select_button) == LOW) && main_count == 1 && digitalRead(scroll_button) == HIGH) {
      delay(300);
      display.clearDisplay();
      highlight_string(" PRINT-A-SKETCH ", 15, 0);
      highlight_string("Print Line", 4, 12);
      string("Print Image", 4, 24);
      string("Settings", 4, 36);
      string("Power: ON", 4, 48);
      display.display();
      menu_id = LINE_MENU;
    }
    //--------------------------------------- Print Image ---------------------------------------------
    if (main_count == 2 && (digitalRead(select_button) == HIGH)) {
      display.clearDisplay();
      highlight_string(" PRINT-A-SKETCH ", 15, 0);
      string("Print Line", 4, 12);
      string("Print Image", 4, 24);
      string("Settings", 4, 36);
      string("Power: ON", 4, 48);
      display.drawRect(1, 22, 127, 11, WHITE);
      display.display();
      delay(1);
    }
    if ((digitalRead(select_button) == LOW) && main_count == 2 && digitalRead(scroll_button) == HIGH) {
      delay(300);
      display.clearDisplay();
      highlight_string(" PRINT-A-SKETCH ", 15, 0);
      string("Print Line", 4, 12);
      highlight_string("Print Image", 4, 24);
      string("Settings", 4, 36);
      string("Power: ON", 4, 48);
      display.display();
      // TO DO: Print image
      disp_printing();
      printImage = true;
    }
  
    //--------------------------------------- Settings --------------------------------------------------
    if (main_count == 3 && (digitalRead(select_button) == HIGH)) {
      display.clearDisplay();
      highlight_string(" PRINT-A-SKETCH ", 15, 0);
      string("Print Line", 4, 12);
      string("Print Image", 4, 24);
      string("Settings", 4, 36);
      string("Power: ON", 4, 48);
      display.drawRect(1, 34, 127, 11, WHITE);
      display.display();
      delay(1);
    }
    if ((digitalRead(select_button) == LOW) && main_count == 3 && digitalRead(scroll_button) == HIGH) {
      delay(300);
      display.clearDisplay();
      highlight_string(" PRINT-A-SKETCH ", 15, 0);
      string("Print Line", 4, 12);
      string("Print Image", 4, 24);
      highlight_string("Settings", 4, 36);
      string("Power: ON", 4, 48);
      display.display();
      menu_id = SETTINGS_MENU;
    }
  
    //------------------------------------- POWER: ON/OFF ------------------------------------------------
    if (main_count == 4 && (digitalRead(select_button) == HIGH)) {
      display.clearDisplay();
      highlight_string(" PRINT-A-SKETCH ", 15, 0);
      string("Print Line", 4, 12);
      string("Print Image", 4, 24);
      string("Settings", 4, 36);
      string("Power: ON", 4, 48);
      display.drawRect(1, 46, 127, 11, WHITE);
      display.display();
      delay(1);
    }
    if ((digitalRead(select_button) == LOW) && main_count == 4 && digitalRead(scroll_button) == HIGH) {
      delay(300);
      display.clearDisplay();
      highlight_string(" PRINT-A-SKETCH ", 15, 0);
      string("Print Line", 4, 12);
      string("Print Image", 4, 24);
      string("Settings", 4, 36);
      highlight_string("Power: OFF", 4, 48);
      display.display();
      delay(500);
      // TO DO : turn off printer
      powerSwitch = true;
    }
  }
  else{ //---------------------------- when printer if OFF -----------------------------------------------   
    display.clearDisplay();
    highlight_string(" PRINT-A-SKETCH ", 15, 0);
    string("POWER: OFF", 4, 24);
    display.display();
    delay(1);
    if ((digitalRead(select_button) == LOW) && (digitalRead(scroll_button) == HIGH)){
      powerSwitch = true;
      display.clearDisplay();
      highlight_string(" PRINT-A-SKETCH ", 15, 0);
      highlight_string("POWER: ON", 4, 24);
      display.display();
      delay(1000);
    }        
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~PRINT LINE MENU~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
void linemenu() {

  //--------------------------------------- Scroll Counter ----------------------------------------
  if (digitalRead(scroll_button) == LOW && digitalRead(select_button) == HIGH) {
    delay(300);
    printline_count++;
    if (printline_count > 2) {
      printline_count = 1;
    }
  }

  //--------------------------------------- SET WIDTH ----------------------------------------------
  if (printline_count == 1 && (digitalRead(select_button) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT-A-SKETCH ", 15, 0);
    string("Set Width", 4, 12);
    string("Print", 4, 24);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(select_button) == LOW) && printline_count == 1 && digitalRead(scroll_button) == HIGH) {
    display.clearDisplay();
    highlight_string(" PRINT-A-SKETCH ", 15, 0);
    highlight_string("Set Width", 4, 12);
    string("Print", 4, 24);
    display.display();
    delay(1);
    menu_id = WIDTH_MENU;
  }

  //-------------------------------------- PRINT ----------------------------------------------------
  if (printline_count == 2 && (digitalRead(select_button) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT-A-SKETCH ", 15, 0);
    string("Set Width", 4, 12);
    string("Print", 4, 24);
    display.drawRect(1, 22, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(select_button) == LOW) && printline_count == 2 && digitalRead(scroll_button) == HIGH) {
    display.clearDisplay();
    highlight_string(" PRINT-A-SKETCH ", 15, 0);
    string("Set Width", 4, 12);
    highlight_string("Print", 4, 24);
    display.display();
    delay(1);
    //TO DO: start printing
    disp_printing();
    printLine = true;
  }

  //-------------------------------------- BACK ----------------------------------------------------
  if ((digitalRead(select_button) == LOW) && (digitalRead(scroll_button) == LOW)) {
    back();
    delay(500);
    back_ = true;
    menu_id = MAIN_MENU;
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ WIDTH MENU ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
void widthmenu() {

  if (digitalRead(scroll_button) == LOW & digitalRead(select_button) == HIGH) {
    delay(300);
    width_count++;
    if (width_count > 5) {
      width_count = 1;
    }
  }

  // WIDTH : 8
  if (width_count == 1 && (digitalRead(select_button) == HIGH)) {
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    string("Width: 1", 4, 12);
    //string("Set Pattern", 4, 22);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(select_button) == LOW) && width_count == 1 && digitalRead(scroll_button) == HIGH) {
    delay(300);
    //option = 1;
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    highlight_string("Width: 1", 4, 12);
    display.display();
    for (int i=0; i<16; i++) {
        lineData[i] = pgm_read_byte(&width_8[i]);
     }
    delay(200);
  }
  
  // WIDTH : 16
  if (width_count == 2 && (digitalRead(select_button) == HIGH)) {
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    string("Width: 2", 4, 12);
    //string("Set Pattern", 4, 22);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(select_button) == LOW) && width_count == 2 & digitalRead(scroll_button) == HIGH) {
    delay(300);
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    highlight_string("Width: 2", 4, 12);
    //string("Set Pattern", 4, 22);
    display.display();
    for (int i=0; i<16; i++) {
        lineData[i] = pgm_read_byte(&width_16[i]);
     }
    delay(200);
  }
  
  // WIDTH : 32
  if (width_count == 3 && (digitalRead(select_button) == HIGH)) {
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    string("Width: 3", 4, 12);
    //string("Set Pattern", 4, 22);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(select_button) == LOW) && width_count == 3 && digitalRead(scroll_button) == HIGH) {
    delay(300);
    //option = 1;
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    highlight_string("Width: 3", 4, 12);
    //string("Set Pattern", 4, 22);
    display.display();
    for (int i=0; i<16; i++) {
        lineData[i] = pgm_read_byte(&width_32[i]);
     }
    delay(200);
  }

  // WIDTH : 64
  if (width_count == 4 && (digitalRead(select_button) == HIGH)) {
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    string("Width: 4", 4, 12);
    //string("Set Pattern", 4, 22);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(select_button) == LOW) && width_count == 4 && digitalRead(scroll_button) == HIGH) {
    delay(300);
    //option = 1;
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    highlight_string("Width: 4", 4, 12);
    //string("Set Pattern", 4, 22);
    display.display();
    for (int i=0; i<16; i++) {
        lineData[i] = pgm_read_byte(&width_64[i]);
     }
    delay(200);
  }

  // WIDTH : 128
  if (width_count == 5 && (digitalRead(select_button) == HIGH)) {
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    string("Width: 5", 4, 12);
    //string("Set Pattern", 4, 22);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(select_button) == LOW) && width_count == 5 && digitalRead(scroll_button) == HIGH) {
    delay(300);
    //option = 1;
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    highlight_string("Width: 5", 4, 12);
    //string("Set Pattern", 4, 22);
    display.display();
    for (int i=0; i<16; i++) {
        lineData[i] = pgm_read_byte(&width_128[i]);
     }
    delay(200);
  }

  // BACK
  if ((digitalRead(select_button) == LOW) && (digitalRead(scroll_button) == LOW)) {
    back();
    delay(500);
    back_ = true;
    menu_id = LINE_MENU;
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ JITTER MENU ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
void jittermenu() {
  if (digitalRead(scroll_button) == LOW && digitalRead(select_button) == HIGH) {
    delay(300);
    jittering_count++;
    if (jittering_count > 2) {
      jittering_count = 1;
    }
  }

  // OFF
  if (jittering_count == 1 && (digitalRead(select_button) == HIGH)) {
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    string("Correct Jitter OFF", 4, 12);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(select_button) == LOW) && jittering_count == 1 && digitalRead(scroll_button) == HIGH) {
    delay(300);
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    highlight_string("Correct Jitter OFF", 4, 12);
    comp_jitter = false;
    display.display();
    delay(200);
  }
  
  // ON
  if (jittering_count == 2 && (digitalRead(select_button) == HIGH)) {
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    string("Correct Jitter ON", 4, 12);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(select_button) == LOW) && jittering_count == 2 && digitalRead(scroll_button) == HIGH) {
    delay(300);
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    highlight_string("Correct Jitter ON", 4, 12);
    comp_jitter = true;
    display.display();
    delay(200);
  }

  // BACK
  if ((digitalRead(select_button) == LOW) && (digitalRead(scroll_button) == LOW)) {
    back();
    delay(800);
    back_ = true;
    menu_id = SETTINGS_MENU;
  }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ SETTINGS MENU ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
void settingsmenu() {
  if (digitalRead(scroll_button) == LOW & digitalRead(select_button) == HIGH) {
    delay(300);
    settings_count++;
    if (settings_count > 2) {
      settings_count = 1;
    }
  }

  
  //----------------------------- JITTER ------------------------------------
  if (settings_count == 1 && (digitalRead(select_button) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT-A-SKETCH ", 15, 0);
    string("Correct Jitter", 4, 12);
    string("Drop Count", 4, 24);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(select_button) == LOW) && settings_count == 1 && digitalRead(scroll_button) == HIGH) {
    display.clearDisplay();
    highlight_string(" PRINT-A-SKETCH ", 15, 0);
    highlight_string("Correct Jitter", 4, 12);
    string("Drop Count", 4, 24);
    display.display();
    delay(1);
    menu_id = JITTER_MENU;
  }

  //--------------------------- DROP COUNT ----------------------------------
  if (settings_count == 2 && (digitalRead(select_button) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT-A-SKETCH ", 15, 0);
    string("Correct Jitter", 4, 12);
    string("Drop Count", 4, 24);
    display.drawRect(1, 22, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(select_button) == LOW) && settings_count == 2 && digitalRead(scroll_button) == HIGH) {
    display.clearDisplay();
    highlight_string(" PRINT-A-SKETCH ", 15, 0);
    string("Correct Jitter", 4, 12);
    highlight_string("Drop Count", 4, 24);
    display.display();
    delay(1);
    menu_id = DROPMENU;
  }
  //---------------------------- BACK ----------------------------------------
  if ((digitalRead(select_button) == LOW) && (digitalRead(scroll_button) == LOW)) {
    back();
    delay(500);
    back_ = true;
    menu_id = 0;
  }
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DROP MENU ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
void dropmenu() {
  
  if (digitalRead(scroll_button) == LOW & digitalRead(select_button) == HIGH) {
    delay(300);
    drop_count++;
    if (drop_count > 5) {
      drop_count = 1;
    }
  }

  // DROPS: 1
  if (drop_count == 1 && (digitalRead(select_button) == HIGH)) {
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    string("Drops: 1", 4, 12);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(select_button) == LOW) && drop_count == 1 & digitalRead(scroll_button) == HIGH) {
    delay(300);
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    highlight_string("Drops: 1", 4, 12);
    display.display();
    n_fire = 1;
    delay(200);
  }

  // DROPS: 2
  if (drop_count == 2 && (digitalRead(select_button) == HIGH)) {
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    string("Drops: 2", 4, 12);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(select_button) == LOW) && drop_count == 2 & digitalRead(scroll_button) == HIGH) {
    delay(300);
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    highlight_string("Drops: 2", 4, 12);
    display.display();
    n_fire = 2;
    delay(200);
  }

  // DROPS: 3
  if (drop_count == 3 && (digitalRead(select_button) == HIGH)) {
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    string("Drops: 3", 4, 12);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }
  
  if ((digitalRead(select_button) == LOW) && drop_count == 3 & digitalRead(scroll_button) == HIGH) {
    delay(300);
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    highlight_string("Drops: 3", 4, 12);
    display.display();
    n_fire = 3;
    delay(200);
  }
  
  // DROPS: 4
  if (drop_count == 4 && (digitalRead(select_button) == HIGH)) {
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    string("Drops: 4", 4, 12);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(select_button) == LOW) && drop_count == 4 & digitalRead(scroll_button) == HIGH) {
    delay(300);
    //option = 1;
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    highlight_string("Drops: 4", 4, 12);
    display.display();
    n_fire = 4;
    delay(200);
  }

  // DROPS: 5
  if (drop_count == 5 && (digitalRead(select_button) == HIGH)) {
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    string("Drops: 5", 4, 12);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(select_button) == LOW) && drop_count == 5 & digitalRead(scroll_button) == HIGH) {
    delay(300);
    //option = 1;
    display.clearDisplay();
    highlight_string(" OPTIONS ", 15, 0);
    highlight_string("Drops: 5", 4, 12);
    display.display();
    n_fire = 5;
    delay(200);
  }

  // BACK
  if ((digitalRead(select_button) == LOW) && (digitalRead(scroll_button) == LOW)) {
    back();
    delay(500);
    back_ = true;
    menu_id = SETTINGS_MENU;
  }
}
