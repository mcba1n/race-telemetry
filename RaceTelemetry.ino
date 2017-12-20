// Author: Brendon McBain
// Date: 20/12/2017
// Description: Receives serial data for rev lights and gear from the F1 2017 racing game.
//              Then this data is used to control x1 Adafruit NeoPixel Strip and x1 7 segment display (common cathode).

#include <Adafruit_NeoPixel.h>
 
#define PIXEL_PIN 2
#define N_LEDS 8
#define NUM_SEGS 7
#define TIMER_MAX 1000
#define SERIAL_DELAY 10

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
const int SEG_PINS[] = {9,8,10,16,14,6,7};   // first pin = g
int sleep_timer = 0;
int sleeping = false;
 
void setup() {
  strip.begin();
  strip.setBrightness(5);
  Serial.begin(9600);
  Serial.print("Connection successful!");
  
  // initialise seg pins
  for (int i=0; i<NUM_SEGS; i++)
    pinMode(SEG_PINS[i], OUTPUT);
}
 
void loop() {
  int DI1, DI2;                  // data-in buffers
  if (Serial.available()) {
     DI1 = Serial.read() - 1;   // rev limiter
     DI2 = Serial.read();       // gear
     int rev_state = round( map(DI2, 0, 100, 0, 8) );  // map percentage to 0-8
     
     updateLights(rev_state);
     updateSegments(DI1);
     
     sleep_timer = 0;
     sleeping = false;
  }
  
  // sleep after 10 seconds of no data
  if (sleep_timer >= TIMER_MAX & !sleeping) {
    updateLights(0);
    updateSegments(-1);
    sleeping = true;
  } else {
      sleep_timer++;
  }
  
  // let the buffer populate
  delay(SERIAL_DELAY);
}
 
void updateLights(int n) {
  uint32_t c;    // temp colour
  
  // set the ON states
  for (int i = 0; i < n; i++) {
     // choose colour band
     if (i <= 1) {
       c = strip.Color(0, 255, 0);  // green
     } else if (i <= 5) { 
       c = strip.Color(255, 0, 0);  // red
     } else {             
       c = strip.Color(0, 0, 255);  // blue
     }
     strip.setPixelColor(i, c);
  }
  
  // set the OFF states
  for (int i = n; i < N_LEDS; i++)
     strip.setPixelColor(i, 0);
     
  strip.show();
}

void updateSegments(int n) {
  byte seg = getSegmentByte(n);
  byte bit_mask = B0000001;
  
  for (int i=0; i<NUM_SEGS; i++) {
    int seg_pin = SEG_PINS[i];
    byte data_out = bit_mask & seg;
    bit_mask <<= 1;
    
    if (data_out > B0000000) digitalWrite(seg_pin, HIGH);
    else digitalWrite(seg_pin, LOW); 
  }
}

byte getSegmentByte(int n) {
  switch (n) {
     case 0:   return B1111110;
     case 1:   return B0110000;
     case 2:   return B1101101;
     case 3:   return B1111001;
     case 4:   return B0110011;
     case 5:   return B1011011;
     case 6:   return B1011111;
     case 7:   return B1110000;
     case 8:   return B1111111;
     case 9:   return B1111011;
     case -1: return B0000000;
  }  // msb = a, lsb = g
}
