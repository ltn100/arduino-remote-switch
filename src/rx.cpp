#ifdef RECEIVE

#include <RF24.h>

#include "common.h"


#define OUTPUT_LED_PIN 2
#define INPUT_SWITCH_PIN 3
#define IRQ_PIN 3

#define LOOP_DELAY 100

// Globals
RF24 g_radio(9, 10);
msg_t g_msg{0};

void interruptFunction();

void setup(void)
{
    //Serial.begin(9600);

    // Set output pins
    pinMode(OUTPUT_LED_PIN, OUTPUT);

    // Set up radio
    g_radio.begin();
    g_radio.enableAckPayload();
    g_radio.enableDynamicPayloads();
    g_radio.openReadingPipe(1, PIPE);
    g_radio.maskIRQ(1, 1, 0);
    g_radio.setPALevel(RF24_PA_MIN);
    g_radio.startListening();
    attachInterrupt(
        digitalPinToInterrupt(IRQ_PIN),
        interruptFunction,
        FALLING);
    g_msg |= MSG_ACK_BIT;
    g_radio.writeAckPayload(1, &g_msg, sizeof(g_msg));
}

void loop(void)
{
    digitalWrite(OUTPUT_LED_PIN, g_msg & MSG_ON_BIT);
    delay(LOOP_DELAY);
}

void interruptFunction()
{
    while (g_radio.available())
    {
        g_radio.read(&g_msg, sizeof(g_msg));
        g_msg |= MSG_ACK_BIT;
        g_radio.writeAckPayload(1, &g_msg, sizeof(g_msg));
    }
}

#endif