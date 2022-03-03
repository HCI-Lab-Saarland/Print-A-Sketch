#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     4 
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; Address is the same for 128x64 display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define MAIN_MENU 0
#define LINE_MENU 1
#define SCANPRINT_MENU 2
#define BLOB_MENU 3
#define BLOB_MENU_2 4
#define ENDLINE_MENU 5
#define WIDTH_MENU 6
#define PATTERN_MENU 7
#define JITTER_MENU 8
#define ANGLE_MENU 9
#define SETTINGS_MENU 10
#define DROPMENU 11

int menu_id = 0;

void mainmenu();
void linemenu();
void scanprintmenu();
void blobmenu();
void blobmenu2();
void endlinemenu();
void widthmenu();
void patternmenu();
void jittermenu();
void anglemenu();
void settingsmenu();
void dropmenu();

int settings_count = 1;
int drop_count = 1;
int n_fire = 2;

//Pins
const int button1 = 18;
const int button2 = 19;
//Button1 Counter

boolean back_ = false;
//User's choice
byte option = 0;
void back();
void blobb();
void no_blobb();

boolean main_menu = true;
volatile int count1 = 1;

boolean printline_menu = false;
int printline_count = 1;
boolean width_menu = false;
int width_count = 1;
boolean pattern_menu = false;
int pattern_count = 1;
boolean jittering_menu = false;
int jittering_count = 1;
boolean comp_jitter = false;

int width = 2;
int pattern = 1;
int angle = 0;

boolean scanprint_menu = false;
int scanprint_count = 1;

boolean blob_menu = false;
int blob_count = 1;

boolean blob_menu_2 = false;
int blob_count_2 = 1;

boolean endline_menu = false;
int endline_count = 1;

boolean angle_menu = false;
int angle_count = 1;

static unsigned long last_interrupt_time = 0;
static unsigned long interrupt_time = 0;

boolean blob_detected = false;
void draw_indicator();

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

//Functions:
void string(String text, int x, int y) {
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(x, y);     // Start at top-left corner
  display.println(text);
}

void highlight_string(String text, int x, int y) {
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(BLACK, SSD1306_WHITE); // Draw white text
  display.setCursor(x, y);     // Start at top-left corner
  display.print(text);
}


void lcd_screen() {

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
      scanprintmenu();
      break;
    case 3:
      blobmenu();
      break;
    case 4:
      blobmenu2();
      break;
    case 5:
      endlinemenu();
      break;
    case 6:
      widthmenu();
      break;
    case 7:
      patternmenu();
      break;
    case 8:
      jittermenu();
      break;
    case 9:
      anglemenu();
      break;
    case 10:
      settingsmenu();
      break;
    case 11:
      dropmenu();
      break;
    default:
      break;
  }
}//lcd_screen function

void back() {
  display.clearDisplay();
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(2, 10);     // Start at top-left corner
  highlight_string(" PRINT'N SKETCH ", 15, 0);
  //string("Back", 4, 12);
  display.drawBitmap(0, 0, myBitmap, 128, 64, WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  display.display();
  delay(1);
}

void blobb() {

  display.clearDisplay();
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(2, 10);     // Start at top-left corner
  highlight_string(" PRINT'N SKETCH ", 15, 0);
  string("BLOB", 4, 12);
  display.display();
  delay(1);
}

void no_blobb() {
  display.clearDisplay();
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(2, 10);     // Start at top-left corner
  highlight_string(" PRINT'N SKETCH ", 15, 0);
  string("NO BLOB", 4, 12);
  display.display();
  delay(1);
}

void disp_printing() {
  display.clearDisplay();
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(2, 10);     // Start at top-left corner
  string("Printing...", 15, 12);
  display.display();
  delay(1);
}

void draw_indicator() {
  display.fillCircle(120, 4, 4, WHITE );
  display.display();
  //delay(1);
}
void mainmenu(){
  if (back_ == true) {
    delay(500);
    back_ = false;
  }
  //Button.1 counter
  if (digitalRead(button1) == LOW && digitalRead(button2) == HIGH) {
    delay(300);
    count1++;
    if (count1 > 4) {
      count1 = 1;
    }
  }
  //print line
  if (count1 == 1 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Print Line", 4, 12);
    string("Print Image", 4, 24);
    string("Scan & Print", 4, 36);
    if (blob_detected) {
      string("Interact with Node", 4, 48);
    }
    else {
      string("Interact with Node", 4, 48);
    }
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  //PRINT LINE
  if ((digitalRead(button2) == LOW) && count1 == 1 && digitalRead(button1) == HIGH) {
    delay(300);
    option = 1;
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    highlight_string("Print Line", 4, 12);
    string("Print Image", 4, 24);
    string("Scan & Print", 4, 36);
    if (blob_detected) {
      string("Interact with Node", 4, 48);
    }
    else {
      string("Interact with Node", 4, 48);
    }
    display.display();
    delay(200);
    main_menu = false;
    printline_menu = true;
    menu_id = 1;
  }

  //Print image
  if (count1 == 2 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Print Line", 4, 12);
    string("Print Image", 4, 24);
    string("Scan & Print", 4, 36);
    if (blob_detected) {
      string("Interact with Node", 4, 48);
    }
    else {
      string("Interact with Node", 4, 48);
    }
    display.drawRect(1, 22, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  //PRINT IMAGE
  if ((digitalRead(button2) == LOW) && count1 == 2 && digitalRead(button1) == HIGH) {
    delay(300);
    option = 2;
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Print Line", 4, 12);
    highlight_string("Print Image", 4, 24);
    string("Scan & Print", 4, 36);
    if (blob_detected) {
      string("Interact with Node", 4, 48);
    }
    else {
      string("Interact with Node", 4, 48);
    }
    display.display();
    Serial.write("B");
    Serial.write(option);
    delay(200);
  }

  //scan and print
  if (count1 == 3 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Print Line", 4, 12);
    string("Print Image", 4, 24);
    string("Scan & Print", 4, 36);
    if (blob_detected) {
      string("Interact with Node", 4, 48);
    }
    else {
      string("Interact with Node", 4, 48);
    }
    display.drawRect(1, 34, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  //SCAN AND PRINT
  if ((digitalRead(button2) == LOW) && count1 == 3 && digitalRead(button1) == HIGH) {
    delay(300);
    option = 3;
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Print Line", 4, 12);
    string("Print Image", 4, 24);
    highlight_string("Scan & Print", 4, 36);
    if (blob_detected) {
      string("Interact with Node", 4, 48);
    }
    else {
      string("Interact with Node", 4, 48);
    }
    display.display();
    delay(200);
    main_menu = false;
    scanprint_menu = true;
    menu_id = 2;
  }

  //blob
  if (count1 == 4 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Print Line", 4, 12);
    string("Print Image", 4, 24);
    string("Scan & Print", 4, 36);
    if (blob_detected) {
      string("Interact with Node", 4, 48);
    }
    else {
      string("Interact with Node", 4, 48);
    }
    display.drawRect(1, 46, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  //blob
  if ((digitalRead(button2) == LOW) && count1 == 4 & digitalRead(button1) == HIGH) {
    delay(300);
    option = 3;
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Print Line", 4, 12);
    string("Print Image", 4, 24);
    string("Scan & Print", 4, 36);
    if (blob_detected) {
      highlight_string("Interact with Node", 4, 48);
      //      string("Settings", 4, 22);
      main_menu = false;
      blob_menu_2 = true;
      menu_id = 4;
    }
    else {
      highlight_string("No Node Detected", 4, 48);
      //      string("Settings", 4, 22);
      display.display();
      delay(1000);
    }
    display.display();
    delay(200);
  }

}
void linemenu() {
  option = 1;
  //Button counter
  if (digitalRead(button1) == LOW && digitalRead(button2) == HIGH) {
    delay(300);
    printline_count++;
    if (printline_count > 4) {
      printline_count = 1;
    }
  }

  //Width
  if (printline_count == 1 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Print", 4, 12);
    string("Set Width", 4, 24);
    string("Set Pattern", 4, 36);
    string("Correct Jitter", 4, 48);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && printline_count == 1 && digitalRead(button1) == HIGH) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    highlight_string("Print", 4, 12);
    string("Set Width", 4, 24);
    string("Set Pattern", 4, 36);
    string("Correct Jitter", 4, 48);
    display.display();
    Serial.write("B");
    Serial.write(option);
    Serial.write(width);
    Serial.write(pattern);
    delay(200);
    main_menu = true;
    printline_menu = false;
    menu_id = 0;
  }

  //Pattern
  if (printline_count == 2 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Print", 4, 12);
    string("Set Width", 4, 24);
    string("Set Pattern", 4, 36);
    string("Correct Jitter", 4, 48);
    display.drawRect(1, 22, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && printline_count == 2 && digitalRead(button1) == HIGH) {
    delay(300);
    //option = 2;
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Print", 4, 12);
    highlight_string("Set Width", 4, 24);
    string("Set Pattern", 4, 36);
    string("Correct Jitter", 4, 48);
    display.display();
    delay(200);
    pattern_menu = true;
    printline_menu = false;
    menu_id = 6;
  }

  //Jittering
  if (printline_count == 3 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Print", 4, 12);
    string("Set Width", 4, 24);
    string("Set Pattern", 4, 36);
    string("Correct Jitter", 4, 48);
    display.drawRect(1, 34, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && printline_count == 3 && digitalRead(button1) == HIGH) {
    delay(300);
    //option = 3;
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Print", 4, 12);
    string("Set Width", 4, 24);
    highlight_string("Set Pattern", 4, 36);
    string("Correct Jitter", 4, 48);
    display.display();
    delay(200);
    jittering_menu = true;
    printline_menu = false;
    menu_id = 7;
  }

  //Save and Print
  if (printline_count == 4 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Print", 4, 12);
    string("Set Width", 4, 24);
    string("Set Pattern", 4, 36);
    string("Correct Jitter", 4, 48);
    display.drawRect(1, 46, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && printline_count == 4 && digitalRead(button1) == HIGH) {
    delay(300);
    //option = 3;
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Print", 4, 12);
    string("Set Width", 4, 24);
    string("Set Pattern", 4, 36);
    highlight_string("Correct Jitter", 4, 48);
    display.display();
    delay(200);
    main_menu = true;
    printline_menu = false;
    menu_id = 8;
  }

  if ((digitalRead(button2) == LOW) && (digitalRead(button1) == LOW)) {
    menu_id = 1;
    main_menu = true;
    printline_menu = false;
    back();
    delay(500);
    back_ = true;
    menu_id = 0;
  }
}

void widthmenu() {
  //Button counter
  if (digitalRead(button1) == LOW & digitalRead(button2) == HIGH) {
    delay(300);
    width_count++;
    if (width_count > 5) {
      width_count = 1;
    }
  }

  //Width
  if (width_count == 1 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Width: 1", 4, 12);
    //string("Set Pattern", 4, 22);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && width_count == 1 && digitalRead(button1) == HIGH) {
    delay(300);
    //option = 1;
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    highlight_string("Width: 1", 4, 12);
    //string("Set Pattern", 4, 22);
    display.display();
    width = 0;
    delay(200);
  }

  if (width_count == 2 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Width: 2", 4, 12);
    //string("Set Pattern", 4, 22);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && width_count == 2 & digitalRead(button1) == HIGH) {
    delay(300);
    //option = 1;
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    highlight_string("Width: 2", 4, 12);
    //string("Set Pattern", 4, 22);
    display.display();
    width = 1;
    delay(200);
  }

  if (width_count == 3 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Width: 3", 4, 12);
    //string("Set Pattern", 4, 22);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && width_count == 3 && digitalRead(button1) == HIGH) {
    delay(300);
    //option = 1;
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    highlight_string("Width: 3", 4, 12);
    //string("Set Pattern", 4, 22);
    display.display();
    width = 2;
    delay(200);
  }

  if (width_count == 4 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Width: 4", 4, 12);
    //string("Set Pattern", 4, 22);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && width_count == 4 && digitalRead(button1) == HIGH) {
    delay(300);
    //option = 1;
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    highlight_string("Width: 4", 4, 12);
    //string("Set Pattern", 4, 22);
    display.display();
    width = 3;
    delay(200);
  }


  if (width_count == 5 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Width: 5", 4, 12);
    //string("Set Pattern", 4, 22);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && width_count == 5 && digitalRead(button1) == HIGH) {
    delay(300);
    //option = 1;
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    highlight_string("Width: 5", 4, 12);
    //string("Set Pattern", 4, 22);
    display.display();
    width = 4;
    delay(200);
  }
  if ((digitalRead(button2) == LOW) && (digitalRead(button1) == LOW)) {
    printline_menu = true;
    width_menu = false;
    back();
    delay(500);
    back_ = true;
    menu_id = 1;
  }
}

void anglemenu() {
  //Button counter
  if (digitalRead(button1) == LOW && digitalRead(button2) == HIGH) {
    delay(300);
    angle_count++;
    if (angle_count > 6) {
      angle_count = 1;
    }
  }

  if (angle_count == 1 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string("PRINTING...", 15, 0);
    string("Angle: 90 L", 4, 12);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && angle_count == 1 && digitalRead(button1) == HIGH) {
    delay(300);
    option = 4;
    display.clearDisplay();
    highlight_string("PRINTING...", 15, 0);
    highlight_string("Angle: 90 L ", 4, 12);
    angle = 0;
    Serial.write("B");
    Serial.write(option);
    Serial.write(angle);
    endline_menu = true;
    angle_menu = false;
    display.display();
    menu_id = 1;
    delay(200);
  }

  if (angle_count == 2 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string("PRINTING...", 15, 0);
    string("Angle: 90 R", 4, 12);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && angle_count == 2 && digitalRead(button1) == HIGH) {
    delay(300);
    option = 4;
    display.clearDisplay();
    highlight_string("PRINTING...", 15, 0);
    highlight_string("Angle: 90 R", 4, 12);
    //string("Set Pattern", 4, 22);
    display.display();
    angle = 1;
    Serial.write("B");
    Serial.write(option);
    Serial.write(angle);
    endline_menu = true;
    angle_menu = false;
    menu_id = 1;
    delay(200);
  }

  if (angle_count == 3 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string("PRINTING...", 15, 0);
    string("Angle: 45 L", 4, 12);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && angle_count == 3 && digitalRead(button1) == HIGH) {
    delay(300);
    option = 4;
    display.clearDisplay();
    highlight_string("PRINTING...", 15, 0);
    highlight_string("Angle: 45 L", 4, 12);
    display.display();
    angle = 2;
    Serial.write("B");
    Serial.write(option);
    Serial.write(angle);
    endline_menu = true;
    angle_menu = false;
    menu_id = 1;
    delay(200);
  }

  if (angle_count == 4 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string("PRINTING...", 15, 0);
    string("Angle: 45 R", 4, 12);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && angle_count == 4 & digitalRead(button1) == HIGH) {
    delay(300);
    option = 4;
    display.clearDisplay();
    highlight_string("PRINTING...", 15, 0);
    highlight_string("Angle: 45 R", 4, 12);
    display.display();
    angle = 3;
    Serial.write("B");
    Serial.write(option);
    Serial.write(angle);
    endline_menu = true;
    angle_menu = false;
    menu_id = 1;
    delay(200);
  }

  if (angle_count == 5 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string("PRINTING...", 15, 0);
    string("Angle: 135 L", 4, 12);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && angle_count == 5 && digitalRead(button1) == HIGH) {
    delay(300);
    option = 4;
    display.clearDisplay();
    highlight_string("PRINTING...", 15, 0);
    highlight_string("Angle: 135 L", 4, 12);
    display.display();
    angle = 4;
    Serial.write("B");
    Serial.write(option);
    Serial.write(angle);
    endline_menu = true;
    angle_menu = false;
    menu_id = 1;
    delay(200);
  }

  if (angle_count == 6 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string("PRINTING...", 15, 0);
    string("Angle: 135 R", 4, 12);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && angle_count == 6 && digitalRead(button1) == HIGH) {
    delay(300);
    option = 4;
    display.clearDisplay();
    highlight_string("PRINTING...", 15, 0);
    highlight_string("Angle: 135 R", 4, 12);
    display.display();
    angle = 5;
    Serial.write("B");
    Serial.write(option);
    Serial.write(angle);
    endline_menu = true;
    angle_menu = false;
    menu_id = 1;
    delay(200);
  }

  if ((digitalRead(button2) == LOW) && (digitalRead(button1) == LOW)) {
    endline_menu = true;
    angle_menu = false;
    back();
    delay(800);
    back_ = true;
    menu_id = 1;
  }
}


void patternmenu() {
  if (digitalRead(button1) == LOW && digitalRead(button2) == HIGH) {
    delay(300);
    pattern_count++;
    if (pattern_count > 2) {
      pattern_count = 1;
    }
  }

  if (pattern_count == 1 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Pattern: Straight", 4, 12);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && pattern_count == 1 && digitalRead(button1) == HIGH) {
    delay(300);
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    highlight_string("Pattern: Straight", 4, 12);
    pattern = 1;
    display.display();
    delay(200);
  }

  if (pattern_count == 2 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Pattern: Wavy", 4, 12);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && pattern_count == 2 && digitalRead(button1) == HIGH) {
    delay(300);
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    highlight_string("Pattern: Wavy", 4, 12);
    pattern = 2;
    display.display();
    delay(200);
  }

  if ((digitalRead(button2) == LOW) && (digitalRead(button1) == LOW)) {
    printline_menu = true;
    pattern_menu = false;
    back();
    delay(800);
    back_ = true;
    menu_id = 1;
  }

}
void endlinemenu() {
  //Button.1 counter
  if (digitalRead(button1) == LOW && digitalRead(button2) == HIGH) {
    delay(300);
    endline_count++;
    if (endline_count > 3) {
      endline_count = 1;
    }
  }

  if (endline_count == 1 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string("PRINTING...", 15, 0);
    string("Add Contact Pad", 4, 12);
    string("Add Angle", 4, 24);
    string("Interact with Node", 4, 36);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && endline_count == 1 && digitalRead(button1) == HIGH) {
    delay(300);
    option = 5;
    display.clearDisplay();
    highlight_string("PRINTING...", 15, 0);
    highlight_string("Add Contact Pad", 4, 12);
    string("Add Angle", 4, 24);
    string("Interact with Node", 4, 36);
    display.display();
    Serial.write("B");
    Serial.write(option);
    delay(200);
  }

  if (endline_count == 2 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string("PRINTING...", 15, 0);
    string("Add Contact Pad", 4, 12);
    string("Add Angle", 4, 24);
    string("Interact with Node", 4, 36);
    display.drawRect(1, 22, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && endline_count == 2 && digitalRead(button1) == HIGH) {
    delay(300);
    display.clearDisplay();
    highlight_string("PRINTING...", 15, 0);
    string("Add Contact Pad", 4, 12);
    highlight_string("Add Angle", 4, 24);
    string("Interact with Node", 4, 36);
    display.display();
    angle_menu = true;
    endline_menu = false;
    delay(200);
    menu_id = 9;
  }

  if (endline_count == 3 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string("PRINTING...", 15, 0);
    string("Add Contact Pad", 4, 12);
    string("Add Angle", 4, 24);
    string("Interact with Node", 4, 36);
    display.drawRect(1, 34, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && endline_count == 3 && digitalRead(button1) == HIGH) {
    delay(300);
    display.clearDisplay();
    highlight_string("PRINTING...", 15, 0);
    string("Add Contact Pad", 4, 12);
    string("Add Angle", 4, 24);
    if (blob_detected) {
      highlight_string("Interact with Node", 4, 36);
      menu_id = 3;
    }
    else {
      highlight_string("No Node Detected", 4, 36);
      delay(400);
    }
    display.display();
    blob_menu = true;
    endline_menu = false;
    delay(200);

  }

  if ((digitalRead(button2) == LOW) && (digitalRead(button1) == LOW)) {
    endline_menu = false;
    main_menu = true;
    back();
    delay(800);
    back_ = true;
    menu_id = 0;
  }
}

void jittermenu() {
  if (digitalRead(button1) == LOW && digitalRead(button2) == HIGH) {
    delay(300);
    jittering_count++;
    if (jittering_count > 2) {
      jittering_count = 1;
    }
  }

  if (jittering_count == 1 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Correct Jitter OFF", 4, 12);
    //    string("Correct Jitter ON", 4, 24);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && jittering_count == 1 && digitalRead(button1) == HIGH) {
    delay(300);
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    highlight_string("Correct Jitter OFF", 4, 12);
    //    string("Correct Jitter ON", 4, 24);
    comp_jitter = false;
    display.display();
    delay(200);
  }

  if (jittering_count == 2 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    //    string("Correct Jitter OFF", 4, 12);
    string("Correct Jitter ON", 4, 12);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && jittering_count == 2 && digitalRead(button1) == HIGH) {
    delay(300);
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    //    string("Correct Jitter OFF", 4, 12);
    highlight_string("Correct Jitter ON", 4, 12);
    comp_jitter = true;
    display.display();
    delay(200);
  }

  if ((digitalRead(button2) == LOW) && (digitalRead(button1) == LOW)) {
    printline_menu = true;
    jittering_menu = false;
    back();
    delay(800);
    back_ = true;
    menu_id = 1;
  }

}
void scanprintmenu() {
  option = 3;
  if (digitalRead(button1) == LOW & digitalRead(button2) == HIGH) {
    delay(300);
    scanprint_count++;
    if (scanprint_count > 2) {
      scanprint_count = 1;
    }
  }

  if (scanprint_count == 1 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Scan", 4, 12);
    string("Print", 4, 24);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && scanprint_count == 1 && digitalRead(button1) == HIGH) {
    delay(300);
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    highlight_string("Scan", 4, 12);
    string("Print", 4, 24);
    display.display();
    byte scan = 0;
    Serial.write("B");
    Serial.write(option);
    Serial.write(scan);
    delay(200);
  }

  if (scanprint_count == 2 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Scan", 4, 12);
    string("Print", 4, 24);
    display.drawRect(1, 22, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && scanprint_count == 2 && digitalRead(button1) == HIGH) {
    delay(300);
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Scan", 4, 12);
    highlight_string("Print", 4, 24);
    display.display();
    byte printt = 1;
    Serial.write("B");
    Serial.write(option);
    Serial.write(printt);
    delay(200);
  }

  if ((digitalRead(button2) == LOW) && (digitalRead(button1) == LOW)) {
    scanprint_menu = false;
    main_menu = true;
    back();
    delay(800);
    back_ = true;
    menu_id = 0;
  }

}

void blobmenu() {
  //Button.1 counter
  if (digitalRead(button1) == LOW && digitalRead(button2) == HIGH) {
    delay(300);
    blob_count++;
    if (blob_count > 3) {
      blob_count = 1;
    }
  }
  //print line
  if (blob_count == 1 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" NODE DETECTED! ", 10, 0);
    string("Connect Endpoint", 4, 12);
    string("Stop & Continue", 4, 24);
    string("Route Around", 4, 36);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  //PRINT LINE
  if ((digitalRead(button2) == LOW) && blob_count == 1 && digitalRead(button1) == HIGH) {
    delay(300);
    option = 7;
    display.clearDisplay();
    highlight_string(" NODE DETECTED! ", 10, 0);
    highlight_string("Connect Endpoint", 4, 12);
    string("Stop & Continue", 4, 24);
    string("Route Around", 4, 36);
    Serial.write("B");
    Serial.write(option);
    display.display();
    delay(200);
  }

  //    //Print image
      if (blob_count == 2 && (digitalRead(button2) == HIGH)) {
        display.clearDisplay();
        highlight_string("NODE DETECTED", 0, 0);
        string("Connect Endpoint", 4, 12);
        string("Stop & Continue", 4, 24);
        string("Route Around", 4, 36);
        display.drawRect(1, 22, 127, 11, WHITE);
        display.display();
        delay(1);
      }
  
      //PRINT IMAGE
      if ((digitalRead(button2) == LOW) && blob_count == 2 & digitalRead(button1) == HIGH) {
        delay(300);
        option = 11;
        display.clearDisplay();
        highlight_string("NODE DETECTED", 0, 0);
        string("Connect Endpoint", 4, 12);
        highlight_string("Stop & Continue", 4, 24);
        string("Route Around", 4, 36);
        display.display();
        Serial.write("B");
        Serial.write(option);
        delay(200);
      }
  
  //    //scan and print
      if (blob_count == 3 && (digitalRead(button2) == HIGH)) {
        display.clearDisplay();
        string("Connect Endpoint", 4, 12);
        string("Stop & Continue", 4, 24);
        string("Route Around", 4, 36);
        display.drawRect(1, 34, 127, 11, WHITE);
        display.display();
        delay(1);
      }
  
      //SCAN AND PRINT
      if ((digitalRead(button2) == LOW) && blob_count == 3 & digitalRead(button1) == HIGH) {
        delay(300);
        option = 8;
        display.clearDisplay();
        highlight_string("NODE DETECTED", 0, 0);
        string("Connect Endpoint", 4, 12);
        string("Stop & Continue", 4, 24);
        highlight_string("Route Around", 4, 36);
        Serial.write("B");
        Serial.write(option);
        display.display();
        delay(200);
      }

  if ((digitalRead(button2) == LOW) && (digitalRead(button1) == LOW)) {
    blob_menu = false;
    main_menu = true;
    back();
    delay(800);
    back_ = true;
    menu_id = 0;
  }
}

void blobmenu2() {
  //Button.1 counter
  if (digitalRead(button1) == LOW && digitalRead(button2) == HIGH) {
    delay(300);
    blob_count_2++;
    if (blob_count_2 > 3) {
      blob_count_2 = 1;
    }
  }
  //print line
  if (blob_count_2 == 1 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" NODE DETECTED! ", 10, 0);
    string("Connect Startpoint", 4, 12);
    string("Continue Trace", 4, 24);
    string("Parallel Line", 4, 36);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  //PRINT LINE
  if ((digitalRead(button2) == LOW) && blob_count_2 == 1 && digitalRead(button1) == HIGH) {
    delay(300);
    option = 6;
    display.clearDisplay();
    highlight_string(" NODE DETECTED! ", 10, 0);
    highlight_string("Connect Startpoint", 4, 12);
    string("Continue Trace", 4, 24);
    string("Parallel Line", 4, 36);
    display.display();
    Serial.write("B");
    Serial.write(option);
    delay(200);
  }

  //    //Print image
      if (blob_count_2 == 2 && (digitalRead(button2) == HIGH)) {
        display.clearDisplay();
        highlight_string("NODE DETECTED", 0, 0);
        string("Connect Startpoint", 4, 12);
        string("Continue Trace", 4, 24);
        string("Parallel Line", 4, 36);
        display.drawRect(1, 22, 127, 11, WHITE);
        display.display();
        delay(1);
      }
  
      //PRINT IMAGE
      if ((digitalRead(button2) == LOW) && blob_count_2 == 2 & digitalRead(button1) == HIGH) {
        delay(300);
        option = 9;
        display.clearDisplay();
        highlight_string("NODE DETECTED", 0, 0);
        string("Connect Startpoint", 4, 12);
        highlight_string("Continue Trace", 4, 24);
        string("Parallel Line", 4, 36);
        Serial.write("B");
        Serial.write(option); 
        display.display();
        delay(200);
      }

  //scan and print
  if (blob_count_2 == 3 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" NODE DETECTED! ", 10, 0);
    string("Connect Startpoint", 4, 12);
    string("Continue Trace", 4, 24);
    string("Parallel Line", 4, 36);
    display.drawRect(1, 34, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  //SCAN AND PRINT
  if ((digitalRead(button2) == LOW) && blob_count_2 == 3 & digitalRead(button1) == HIGH) {
    delay(300);
    option = 11;
    display.clearDisplay();
    highlight_string(" NODE DETECTED! ", 10, 0);
    string("Connect Startpoint", 4, 12);
    string("Continue Trace", 4, 24);
    highlight_string("Parallel Line", 4, 36);
    display.display();
    Serial.write("B");
    Serial.write(option);
    delay(200);
  }

  if ((digitalRead(button2) == LOW) && (digitalRead(button1) == LOW)) {
    blob_menu_2 = false;
    //main_menu = true;
    back();
    delay(800);
    back_ = true;
    menu_id = 0;

  }
}

void settingsmenu() {

  if (digitalRead(button1) == LOW & digitalRead(button2) == HIGH) {
    delay(300);
    settings_count++;
    if (settings_count > 1) {
      settings_count = 1;
    }
  }
  //print line
  if (blob_count == 1 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Drop Count", 4, 12);
    //string("Round Around", 4, 22);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  //PRINT LINE
  if ((digitalRead(button2) == LOW) && blob_count == 1 & digitalRead(button1) == HIGH) {
    delay(300);
    option = 7;
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    highlight_string("Drop Count", 4, 12);
    //string("Route Around", 4, 22);
    menu_id = 11;
    display.display();
    delay(200);
  }

  if ((digitalRead(button2) == LOW) && (digitalRead(button1) == LOW)) {
    printline_menu = true;
    width_menu = false;
    back();
    delay(500);
    back_ = true;
    menu_id = 0;
  }
}

void dropmenu() {

  //Button counter
  if (digitalRead(button1) == LOW & digitalRead(button2) == HIGH) {
    delay(300);
    drop_count++;
    if (drop_count > 5) {
      drop_count = 1;
    }
  }

  if (drop_count == 1 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Drops: 1", 4, 12);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && drop_count == 1 & digitalRead(button1) == HIGH) {
    delay(300);
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    highlight_string("Drops: 1", 4, 12);
    display.display();
    n_fire = 1;
    delay(200);
  }

  if (drop_count == 2 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Drops: 2", 4, 12);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && drop_count == 2 & digitalRead(button1) == HIGH) {
    delay(300);
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    highlight_string("Drops: 2", 4, 12);
    display.display();
    n_fire = 2;
    delay(200);
  }

  if (drop_count == 3 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Drops: 3", 4, 12);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && drop_count == 3 & digitalRead(button1) == HIGH) {
    delay(300);
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    highlight_string("Drops: 3", 4, 12);
    display.display();
    n_fire = 3;
    delay(200);
  }

  if (drop_count == 4 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Drops: 4", 4, 12);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && drop_count == 4 & digitalRead(button1) == HIGH) {
    delay(300);
    //option = 1;
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    highlight_string("Drops: 4", 4, 12);
    display.display();
    n_fire = 4;
    delay(200);
  }


  if (drop_count == 5 && (digitalRead(button2) == HIGH)) {
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    string("Drops: 5", 4, 12);
    display.drawRect(1, 10, 127, 11, WHITE);
    display.display();
    delay(1);
  }

  if ((digitalRead(button2) == LOW) && drop_count == 5 & digitalRead(button1) == HIGH) {
    delay(300);
    //option = 1;
    display.clearDisplay();
    highlight_string(" PRINT'N SKETCH ", 15, 0);
    highlight_string("Drops: 5", 4, 12);
    display.display();
    n_fire = 5;
    delay(200);
  }
  if ((digitalRead(button2) == LOW) && (digitalRead(button1) == LOW)) {
    printline_menu = true;
    width_menu = false;
    back();
    delay(500);
    back_ = true;
    menu_id = 10;
  }
}
