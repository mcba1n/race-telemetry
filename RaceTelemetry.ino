// *******************************************************************************************
// **** Author: Brendon McBain
// **** Date: 5/12/2017
// **** Description: This sketch receives incoming serial data for rev lights
// ****              and then updates the LED states, given by the UDP server for F1 2017.
// *******************************************************************************************

#define NUM_STATES 10
#define PIN_SHIFT 2

void setup() {
  Serial.begin(9600);
  Serial.print("Connection successful!");
  
  // initialise digital pins as outputs
  for (int i = 0; i < NUM_STATES; i++) {
     pinMode(i + PIN_SHIFT, OUTPUT);
  }
}

void loop() {
  int input;    // buffer
  
  if (Serial.available()) {
     input = Serial.read();            // incoming data (1 byte)
     int rev_per = round(input / 10);  // map percentage to 0-10
     
     // set the ON states
     for (int i = 0; i < rev_per; i++) {
         digitalWrite(i + PIN_SHIFT, HIGH);
     }
     
     // set the OFF states
     for (int i = rev_per; i < NUM_STATES; i++) {
         digitalWrite(i + PIN_SHIFT, LOW);
     }
  }
}
