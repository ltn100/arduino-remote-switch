#include <Arduino.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>

RF24 radio(9,10); // NRF24L01 used SPI pins + Pin 9 and 10 on the NANO

const uint64_t pipes[2] = {
  0x9909351a76db, // Input  -> Output
  0xe4c4e68770e4  // Output -> Input
};

#define EXT_LED 2
#define SwitchPin 3

#define INPUT_MODULE
//#define OUTPUT_MODULE

#define LOOP_DELAY 5

unsigned int communicate(unsigned int input, unsigned int timeout=0) {
  const unsigned int delay_time = 5;
  unsigned int output = 0x0;

  radio.stopListening();
  delay(delay_time);
  radio.write(&input, sizeof(input));
  

  radio.startListening();
  delay(delay_time);
  unsigned int count = 0;

  while(!radio.available()) {
    if (count*delay_time > timeout) {
      return 0x0;
    }
    delay(delay_time);
    count++;
  }
  radio.read(&output, sizeof(output));
  return output;
}

#ifdef INPUT_MODULE
///////////////////////////////////////////
// Input:
//  This module detects current and has
//  a manual switch.
///////////////////////////////////////////
void setup(void){
  pinMode(EXT_LED, OUTPUT);
  pinMode(SwitchPin, INPUT_PULLUP);
  digitalWrite(SwitchPin,HIGH);
  //Serial.begin(9600);

  radio.begin();
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1, pipes[1]);
  radio.setPALevel(RF24_PA_MIN);
}

void loop(void){

  unsigned int msg = (!digitalRead(SwitchPin)) & 0x1;
  //Serial.print("Wrote: ");
  //Serial.print(msg);
  //Serial.print("\n");
  msg = communicate(msg);
  digitalWrite(EXT_LED, msg & 0x1);
  //Serial.print("Read: ");
  //Serial.print(msg);
  //Serial.print("\n");  
}
#else
///////////////////////////////////////////
// Output:
//  This module switches the vaccum
///////////////////////////////////////////
void setup(void){
  pinMode(EXT_LED, OUTPUT);
  //Serial.begin(9600);

  radio.begin();
  radio.openReadingPipe(1, pipes[0]);
  radio.openWritingPipe(pipes[1]);
  radio.setPALevel(RF24_PA_MIN);
}

void loop(void){

  unsigned int msg = digitalRead(EXT_LED) & 0x1;
  //Serial.print("Wrote: ");
  //Serial.print(msg);
  //Serial.print("\n");
  msg = communicate(msg, 500);
  //Serial.print("Read: ");
  //Serial.print(msg);
  //Serial.print("\n"); 
  digitalWrite(EXT_LED, msg & 0x1);
}
#endif
