#include "Arduino.h"
#include "xaar128.h"
#include <PMW3360.h>
#include "Adafruit_NeoPixel.h"
#include "OLED_display.h"

#define RGB_LED 13
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, RGB_LED, NEO_GRB + NEO_KHZ800);

//-------
typedef unsigned char uchar;
typedef unsigned int uint;
#define ABS(x) ((x)<0?-(x):(x))
#define SIGN(x) ((x)<0?-1:+1)
#define SHIFT_IN_BYTE 0x00 // bit values to be shifted in the array
#define SHIFT_L(ptr,shift) (*(uchar*)(ptr)<<(shift)|*((uchar*)(ptr)+1)>>(8-(shift)))
#define SHIFT_R(ptr,shift) (*(uchar*)(ptr)>>(shift)|*((uchar*)(ptr)-1)<<(8-(shift)))
#define SHIFT(ptr,shift) ((shift)>=0?SHIFT_L(ptr,shift):SHIFT_R(ptr,-(shift)))


//-------
PMW3360 sensor;

int16_t x = 0;
int16_t y = 0;

int distance_x = 0.0;
int distance_y = 0.0;
int shift_n = 0;
float shift_nn = 0;
int inter_line_dist = 20;
int newdata_timeout = 3000;
static boolean power_on = false;
static boolean printing = false;
boolean print_newline = false;
boolean print_newline2 = false;
boolean print_newline3 = false;

boolean blob = false;

Xaar128 xaar128;

float steps = 0.0;
float fails = 0.0;

#define XAAR_BEGIN SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE2))
#define XAAR_END   SPI.endTransaction()

int sensor_ss = SENSOR_SS; 

byte IMAGE1[8];
byte IMAGE2[8];

//----------------- Setup -----------------------------------
void setup (void) {   
  Serial.begin(115200);
  //Wire.begin(); 
  Serial.println("Restarted");
  
  sensor.begin(sensor_ss, 12000); // to set CPI (Count per Inch), pass it as the second parameter
  if(sensor.begin(sensor_ss))  // 10 is the pin connected to SS of the module.
    Serial.println("Sensor initialization successed");
  else
    Serial.println("Sensor initialization failed");
  
  sensor.setCPI(12000);    // or, you can set CPI later by calling setCPI();  

  xaar128.init();

  //1MHz square wave for Xaar CLK
  TCCR1A = _BV(COM1A0);             
  OCR1A = 7;                         
  TCCR1B = _BV(WGM12) | _BV(CS10);
  
  //RGB LED
  strip.begin();
  strip.setBrightness(200);
  strip.show(); // Initialize all pixels to 'off'

  colorWipe(strip.Color(255, 255, 255), 1);  

  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP); 
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  lcd_screen();
}

//---------------------- Loop --------------------------------
void loop (void) {  
  if (Serial.available())
  {
      receiveBytes();
  }
  lcd_screen();
  if(blob_detected){
    draw_indicator();
  }
}

//------------------------Sensor Update-----------------------
void get_new_position(){
  PMW3360_DATA data = sensor.readBurst();
    x = data.dx;
    y = data.dy;
}

//------------ Receive Data ----------------------------------

void receiveBytes() {
    char startMarker = '<';
    char endMarker = '>';
    char blobData = 'B';
    char rb;
    while (Serial.available()) {              
          while (power_on == true) {       
            // Wait for new data
            long int start = millis();
            while(!Serial.available()){
              long int now = millis();
                if (((now - start)> newdata_timeout))
                {
                  //Serial.println("No new data rx");
                  printSummary(fails, steps);
                  distance_y = 0;
                  return;
                }
              }
            delay(3);

            // Check no. of bytes received
            if (Serial.available() == 1){
              rb = Serial.read();
              // POWER DOWN 
              if (rb ==endMarker){
                //Serial.println("END");
                delay(100);
                xaar128.powerDown();
                power_on = false;
                  fails = 0;
                  steps = 0;
                }               
              }
            else if (Serial.available() == 0){
              //Serial.println("NO DATA");
            }
            else{                          
              long int start2 = millis();
              while(Serial.available() < 17){
                long int now2 = millis();
                if ((now2- start2) > newdata_timeout)
                 {
                    //flush serial rx buffer
                    while(Serial.available() > 0) {
                        char t = Serial.read();
                      }
                    //Serial.println("No new data received");
                    printSummary(fails, steps);
                    distance_y = 0;   
                    return;                    
                 }
              }

              //16 bytes received, fire printhead
              Serial.readBytes(IMAGE2, 8);
              Serial.readBytes(IMAGE1, 8);
              
              blob = Serial.read();
              if (blob == 0){
                blob_detected = false;
                //lcd_screen();
              }
              else if (blob == 1)
              {
                //lcd_screen();
                blob_detected = true;
                draw_indicator();                
              }

              if((digitalRead(button2) == LOW) & (digitalRead(button1) == LOW)){
                Serial.write("C");
                display.clearDisplay();
                highlight_string(" PRINT'N SKETCH ", 15, 0);
                string("STOPPED PRINTING!", 4, 17);
                //string("Print an image", 4, 12);
                display.drawRect(1, 15, 127, 11, WHITE);
                display.display();
                delay(1000);
                printing = false;
                endline_menu = true;
                menu_id = 5;
                main_menu = false;

                return;                
              }
              else
              {
              //Send ack, to receive next line
              Serial.write("A"); 
              }

              while(!print_newline){
                                        
                get_new_position();
                distance_y += y;
                distance_x += abs(x);
                if(distance_x >= inter_line_dist){
                  print_newline = true;

                  if (comp_jitter){
                    shift_n = -distance_y/60;
                    if(shift_n != 0){
                      shift_data(IMAGE1, IMAGE2, shift_n);          
                    }
                  }
                  
                    //Send data to printhead
                    XAAR_BEGIN;
                   
                    digitalWrite(nSS2, LOW);
                    digitalWrite(nSS1, HIGH);
                    xaar128.loadBuffer64(IMAGE2);
                  
                    digitalWrite(nSS2, HIGH);
                    digitalWrite(nSS1, LOW);
                    xaar128.loadBuffer64(IMAGE1);
                  
                    // Exit with both chips disabled (HIGH).
                    digitalWrite(nSS1, HIGH);
      
                    XAAR_END; //Data sent
                  
                  for (int i = 0; i<n_fire; i++){
                    if (!xaar128.fire()) fails++;
                    steps++;
                  }
                  
                  distance_x = 0;
                  while(!print_newline2){
                    get_new_position();
                    distance_y += y;
                    distance_x += abs(x);
                    if(distance_x >= inter_line_dist){
                        print_newline2 = true;
                    }                         
                  }

                  if(print_newline2){
                    for (int i = 0; i<n_fire; i++){
                      if (!xaar128.fire()) fails++;
                        steps++;
                      }                                     
                  }
                  print_newline2 = false;

                  distance_x = 0;
                  while(!print_newline3){
                    get_new_position();
                    distance_y += y;
                    distance_x += abs(x);
                    if(distance_x >= inter_line_dist){
                        print_newline3 = true;
                    }                         
                  }

                  if(print_newline3){
                    for (int i = 0; i<n_fire; i++){
                      if (!xaar128.fire()) fails++;
                        steps++;
                      }                                     
                  }
                  print_newline3 = false;
                }
              }
              
              print_newline = false;
              distance_x = 0;          
            }
          } // while power_on True
          
          if (power_on == false)
          {       
            rb = Serial.read();
            // POWER UP
            if (rb == blobData){
              char zero = '0';
              byte blob_ = Serial.read();
              if (blob_ == zero){
                blob_detected = false;
                
              }
              else{
                blob_detected = true;
                draw_indicator();
              }
            }

            if(rb == startMarker) {        
              //Serial.println("START");
              xaar128.powerUp();
              delay(1000);
              power_on = true;
              //blob_detected = false;

              get_new_position();
              distance_y = 0;
              colorWipe(strip.Color(255, 255, 255), 1);    
              }
            }  // while power_on False
            
        } //while serial available
        
}

//----------------------Print Summary-----------------------
void printSummary(float fails, float steps) {
 // Serial.println("Done.");
  float f = fails / steps;
//  Serial.print("Steps: ");
//  Serial.print(steps);
//  Serial.print("    Fails: ");
//  Serial.print(fails);
//  Serial.print("    Failure Rate: ");
//  Serial.println(f, 4);
  fails = 0;
  steps = 0;
//  done();
}
//-----------------------SHIFT -------------------------------
void shift_data(byte data_1[8],  byte data_2[8], int shift_n){
  unsigned char data[16];
  unsigned char data_res[16];
  for (int k=0; k<8; k++){
    data[k] = data_2[k];
  }
  for (int k=0; k<8; k++){
    data[k+8] = data_1[k];
  }
  bit_array_shift(data_res, data, 16, shift_n);
  for (int i=0; i<8; i++){
    IMAGE1[i] = data_res[i+8];
    IMAGE2[i] = data_res[i];
  }
}

void bit_array_shift(void *dest_arr, void *src_arr, uint arr_size, int shift)
{
  if(!dest_arr||!src_arr) return; // do nothing for NULL pointers

  int step = 2*(shift>=0)-1;
  int shift_bytes = shift/8;
  int shift_bits = shift%8;
  char *dest_beg = shift>=0?(char*)dest_arr:(char*)dest_arr+arr_size-1;
  char *src_beg = shift>=0?(char*)src_arr:(char*)src_arr+arr_size-1;
  char *src_end = (shift>=0?(char*)src_arr+arr_size-1:(char*)src_arr) - shift_bytes;

  if((uint)ABS(shift_bytes)<arr_size)
  { static char last_byte[3] = {SHIFT_IN_BYTE,0,SHIFT_IN_BYTE};
    while(src_beg!=src_end) { *dest_beg = SHIFT(src_beg+shift_bytes,shift_bits);
    dest_beg+=step;
    src_beg+=step;
    };
    last_byte[1] = *(src_beg+shift_bytes);
    *dest_beg = SHIFT(last_byte+1,shift_bits);
    dest_beg+=step;
  }
  else
    shift_bytes = SIGN(shift_bytes)*arr_size;

  while(shift_bytes)
  dest_beg[shift_bytes-=step] = SHIFT_IN_BYTE;

  return;
};
// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}
