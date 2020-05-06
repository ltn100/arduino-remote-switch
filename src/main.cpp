#include <Arduino.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>

#define SwitchPin 8 // Arcade switch is connected to Pin 8 on NANO
int SentMessage[1] = {0}; // Used to store value before being sent through the NRF24L01
int RecvMessage[1] = {0};

RF24 radio(9,10); // NRF24L01 used SPI pins + Pin 9 and 10 on the NANO

const uint64_t pipes[2] = {
  0x9909351a76db, // Input  -> Output
  0xe4c4e68770e4  // Output -> Input
};

#define EXT_LED 2
#define SwitchPin 3

#define TX




#ifdef TX
void setup(void){
  pinMode(EXT_LED, OUTPUT);
  pinMode(SwitchPin, INPUT_PULLUP);
  digitalWrite(SwitchPin,HIGH);
  Serial.begin(9600);

  radio.begin();
  radio.openWritingPipe(pipes[0]);
  radio.setPALevel(RF24_PA_MIN);
}

void loop(void){
  delay(100);
  SentMessage[0] = digitalRead(SwitchPin);
  radio.write(SentMessage, 1);
  Serial.print("Wrote: ");
  Serial.print(SentMessage[0]);
  Serial.print("\n");
  digitalWrite(EXT_LED, SentMessage[0]);

}
#else

void setup(void){
  pinMode(EXT_LED, OUTPUT);
  Serial.begin(9600);

  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.setPALevel(RF24_PA_MIN);
  SentMessage[0] = 0x1;
  radio.startListening();
}

void loop(void){
  delay(50);

  Serial.print("Waiting for radio...\n");
  while(!radio.available()); // wait for radio
  Serial.print("Got radio...\n");

  radio.read(SentMessage, 1);
  Serial.print("Read: ");
  Serial.print(SentMessage[0]);
  Serial.print("\n");

  digitalWrite(EXT_LED, SentMessage[0]);
}


#endif


// void setup(void){
//   pinMode(LED_BUILTIN, OUTPUT);
//   Serial.begin(9600);

//   radio.begin();
//   radio.openWritingPipe(pipe);
//   radio.openReadingPipe(1, pipe);
//   radio.setPALevel(RF24_PA_MIN);
//   radio.startListening();
//   SentMessage[0] = 0x1;
// }

// void loop(void){
//   delay(500);

//   radio.write(SentMessage, 1);
//   Serial.print("Wrote: ");
//   Serial.print(SentMessage[0]);
//   Serial.print("\n");

//   delay(500);
  
//   Serial.print("Waiting for radio...\n");
//   //while(!radio.available()); // wait for radio
//   Serial.print("Got radio...\n");

//   radio.read(RecvMessage, 1);
//   Serial.print("Read: ");
//   Serial.print(RecvMessage[0]);
//   Serial.print("\n");

//   digitalWrite(LED_BUILTIN, RecvMessage[0]);

//   SentMessage[0] = 1 - RecvMessage[0];

//   delay(1000);
// }