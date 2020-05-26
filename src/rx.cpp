#ifdef RECEIVE

#include <RF24.h>

#include "common.h"


#define IRQ_PIN 2
#define CE_PIN 9
#define CS_PIN 10
#define OUTPUT_LED_PIN 3
#define OUTPUT_SWITCH_PIN 4

#define LOOP_DELAY 100
#define PERSIST_TIME 5000  // ms

// Globals
RF24 g_radio(CE_PIN, CS_PIN);
msg_t g_msg {0};
int g_persist_time {0};

void interruptFunction();

void setup(void)
{
    #ifdef DEBUG
    Serial.begin(9600);
    #endif

    // Set output pins
    pinMode(OUTPUT_LED_PIN, OUTPUT);
    pinMode(OUTPUT_SWITCH_PIN, OUTPUT);
    digitalWrite(OUTPUT_SWITCH_PIN, HIGH);

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
    bool on = g_msg & MSG_ON_BIT;

    if (on) {
        g_persist_time = PERSIST_TIME;
    } else {
        if (g_persist_time > 0) {
            on = true;
            g_persist_time -= LOOP_DELAY;
        }
    }

    digitalWrite(OUTPUT_LED_PIN, on);
    digitalWrite(OUTPUT_SWITCH_PIN, !on);

    delay(LOOP_DELAY);
}

void interruptFunction()
{
    while (g_radio.available())
    {
        g_radio.read(&g_msg, sizeof(g_msg));

        #ifdef DEBUG
         Serial.print("Read: ");
        Serial.print(g_msg);
        Serial.print("\n");
        #endif

        g_msg |= MSG_ACK_BIT;
        g_radio.writeAckPayload(1, &g_msg, sizeof(g_msg));

        #ifdef DEBUG
        Serial.print("ACK: ");
        Serial.print(g_msg);
        Serial.print("\n");
        #endif
    }
}

#endif
