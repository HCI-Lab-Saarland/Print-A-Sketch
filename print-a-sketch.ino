/***************************************************
 This is a library for Print-A-Sketch, an open-source handheld printer prototype for sketching circuits and sensors.
 For more information on the project, visit our website.
 Designed and tested to work with Arduino Uno and MEGA2560.
 
 Written by Narjes Pourjafarian, Marion Koelle, Fjolla Mjaku,
			      Paul Strohmeier and Juergen Steimle (Saarland University)

 Xaar128 files are a modified version of the library written by https://github.com/gkyle.
			
 MIT license, all text above must be included in any redistribution
 ****************************************************/

#include "Arduino.h"
#include "xaar128.h"
#include "OLED_display.h"
#include <PMW3360.h>

#define XAAR_BEGIN SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE2))
#define XAAR_END   SPI.endTransaction()

//Bit shifting
typedef unsigned char uchar;
typedef unsigned int uint;
#define ABS(x) ((x)<0?-(x):(x))
#define SIGN(x) ((x)<0?-1:+1)
#define SHIFT_IN_BYTE 0x00 // bit values to be shifted in the array
#define SHIFT_L(ptr,shift) (*(uchar*)(ptr)<<(shift)|*((uchar*)(ptr)+1)>>(8-(shift)))
#define SHIFT_R(ptr,shift) (*(uchar*)(ptr)>>(shift)|*((uchar*)(ptr)-1)<<(8-(shift)))
#define SHIFT(ptr,shift) ((shift)>=0?SHIFT_L(ptr,shift):SHIFT_R(ptr,-(shift)))

PMW3360 sensor;
Xaar128 xaar128;

//Positioning variables
int16_t x = 0; // last read deltaX from sensor
int16_t y = 0; // last read deltaY from sensor
int distance_x = 0.0; //accumulated distance x
int distance_y = 0.0; //accumulated distance y
int shift_n = 0; //number of bits to shift
int inter_line_dist = 60; //distance_x between lines
boolean print_newline = false; //true when next line can be printed

float steps = 0.0;
float fails = 0.0;

byte buf1[8]; //stores pixel data for nozzles 0-63
byte buf2[8]; //stores pixel data for nozzles 64-127

//---------------------------- SETUP -----------------------------------
void setup (void) {   
  Serial.begin(115200); 
  Serial.println("Restarted");
  
  sensor.begin(SENSOR_SS, 12000); // CPI = 12000
  if(sensor.begin(SENSOR_SS))
    Serial.println("Sensor initialization successed");
  else
    Serial.println("Sensor initialization failed");
  
  sensor.setCPI(12000);

  xaar128.init();

  //1MHz square wave for Xaar CLK
  TCCR1A = _BV(COM1A0);             
  OCR1A = 7;                         
  TCCR1B = _BV(WGM12) | _BV(CS10);   

  pinMode(scroll_button, INPUT_PULLUP);
  pinMode(select_button, INPUT_PULLUP); 
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
}

//------------------------------ LOOP ------------------------------------
void loop (void) {

  if (powerSwitch){
    if (printer_on){
      xaar128.powerDown();
      Serial.println("Power DOWN");
      printer_on = false;
      menu_id = 0;
    }
    else{
      xaar128.powerUp();
      Serial.println("Power UP");
      printer_on = true;
    }
    powerSwitch = false;
  }
  
  if (printImage){
    print_image(); 
  }
  if (printLine){
    print_line(lineData);
  }
  
  refresh_display();
}
//------------------------- PRINT IMAGE --------------------------------
void print_image(){
  //Get data line by line from hardcoded image  
  for(int row_px= 0; row_px<COLS/2; row_px++){
     for (int i=0; i<8; i++) {
       buf2[i] = pgm_read_byte(&PATTERN2[row_px*2][i]);
       buf1[i] = pgm_read_byte(&PATTERN2[row_px*2+1][i]);
     }

      //Wait until distance_x reaches the threshold inter_line_dist
      while(!print_newline){
        get_new_position();
        distance_y += y;
        distance_x += x;
        if(distance_x >= inter_line_dist){
          print_newline = true;
          distance_x = 0;
        }
       }

       if ((digitalRead(select_button) == LOW) && (digitalRead(scroll_button) == LOW)){
          print_newline = false;
          distance_x = 0;
          distance_y = 0;
          printImage = false;
          disp_cancelled();
          row_px = COLS/2;
          delay(1000);
          return;
       }
     //if printer has moved on the y-axis, shift bits to compensate for it
      if (comp_jitter){
          shift_n = -distance_y/60;
          if(shift_n != 0){
            shift_data(buf1, buf2, shift_n);          
           }
         }
       
      if(print_newline){        
        //Load data to printer through SPI
        XAAR_BEGIN;
        digitalWrite(nSS2, LOW);
        digitalWrite(nSS1, HIGH);
        xaar128.loadBuffer64(buf2);
      
        digitalWrite(nSS2, HIGH);
        digitalWrite(nSS1, LOW);
        xaar128.loadBuffer64(buf1);
      
        digitalWrite(nSS1, HIGH);
        XAAR_END;

        //Fire n_fire times
        for (int i = 0; i<n_fire; i++){
          if (!xaar128.fire()) fails++;
          delayMicroseconds(180);
          steps++;
         }
         print_newline = false;
      }
  }
  Serial.print("Failure Rate: ");
  Serial.println(fails/steps);
  Serial.println("Finished printing image");
  fails = 0;
  steps = 0;
  distance_y = 0;
  printImage = false;
}

//------------------------------ PRINT LINE ---------------------------------
void print_line(byte data[16]){
  int line_length = 5000;
  
  //load data from print line buffer data_[16]
  for(int k= 0; k<line_length; k++){
     for (int i=0; i<8; i++) {
       buf2[i] = data[i];
       buf1[i] = data[i+8];
     }
     
      //Wait until distance_x reaches the threshold inter_line_dist
      while(!print_newline){
        get_new_position();
        distance_y += y;
        distance_x += x;
        if(distance_x >= inter_line_dist){
          print_newline = true;
          distance_x = 0;
        }

       if ((digitalRead(select_button) == LOW) && (digitalRead(scroll_button) == LOW)){
          print_newline = false;
          distance_x = 0;
          distance_y = 0;
          printLine = false;
          k = line_length;
          disp_cancelled();
          delay(1000);
          return;
       }
        
        //if printer has moved on the y-axis, shift bits to compensate for it
        if (comp_jitter & print_newline){
          shift_n = -distance_y/60;
          if(shift_n != 0){
            shift_data(buf1, buf2, shift_n);          
           }
         }
       }
       
      if(print_newline){
        
        //Load data to printer through SPI
        XAAR_BEGIN;
        digitalWrite(nSS2, LOW);
        digitalWrite(nSS1, HIGH);
        xaar128.loadBuffer64(buf2);
      
        digitalWrite(nSS2, HIGH);
        digitalWrite(nSS1, LOW);
        xaar128.loadBuffer64(buf1);
      
        digitalWrite(nSS1, HIGH);
        XAAR_END;
        
        //Fire n_fire times
        for (int i = 0; i<n_fire; i++){
          if (!xaar128.fire()) fails++;
          delayMicroseconds(180);
          steps++;
         }

         print_newline = false;
      }
  }

  Serial.print("Failure Rate: ");
  Serial.println(fails/steps);
  Serial.println("Finished printing line");
  fails = 0;
  steps = 0;
  distance_y = 0;
  printLine = false;
}

//------------------------ SENSOR UPDATE -----------------------
void get_new_position(){
  PMW3360_DATA data = sensor.readBurst();
  x = data.dx;
  y = data.dy;
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
    buf1[i] = data_res[i+8];
    buf2[i] = data_res[i];
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
