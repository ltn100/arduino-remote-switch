#ifdef TRANSMIT

#include <RF24.h>
#include <ACS712.h>

#include "common.h"


#define OUTPUT_LED_PIN 4
#define INPUT_SWITCH_PIN 3
#define INPUT_CURRNET_SENSOR_PIN A0

#define CURRENT_THRESHOLD 0.3
#define CURRENT_FREQ 50  // 50Hz

#define LOOP_DELAY 100

// Globals
RF24 g_radio(9, 10);
ACS712 g_current_sensor(ACS712_30A, INPUT_CURRNET_SENSOR_PIN);

void setup(void)
{
    #ifdef DEBUG
    Serial.begin(9600);
    #endif

    // Set input pins
    pinMode(INPUT_SWITCH_PIN, INPUT_PULLUP);
    digitalWrite(INPUT_SWITCH_PIN, HIGH);
    pinMode(INPUT_CURRNET_SENSOR_PIN, INPUT);

    // Set output pins
    pinMode(OUTPUT_LED_PIN, OUTPUT);
   
    // Set up radio
    g_radio.begin();
    g_radio.enableAckPayload();
    g_radio.enableDynamicPayloads();
    g_radio.openWritingPipe(PIPE);
    g_radio.setPALevel(RF24_PA_MIN);
    g_radio.stopListening();

    g_current_sensor.calibrate();
}

void loop(void)
{
    msg_t msg {0};
    
    bool switch_on = !digitalRead(INPUT_SWITCH_PIN);
    float current = g_current_sensor.getCurrentAC(CURRENT_FREQ);

    if (switch_on || current >= CURRENT_THRESHOLD)
    {
        msg |= MSG_ON_BIT;
    }

    #ifdef DEBUG
    Serial.print("Current: ");
    Serial.print(current);
    Serial.print("\n");

    Serial.print("Switch: ");
    Serial.print(switch_on);
    Serial.print("\n");

    Serial.print("Wrote: ");
    Serial.print(msg);
    Serial.print("\n");
    #endif

    bool ack = g_radio.write(&msg, sizeof(msg));

    if (!ack or !g_radio.available())
    {
        // No ACK or Empty ACK
        msg = MSG_NULL;
    }
    else
    {
        while (g_radio.available())
        {
            g_radio.read(&msg, sizeof(msg));
        }
    }

    #ifdef DEBUG
    Serial.print("ACK: ");
    Serial.print(msg);
    Serial.print("\n");
    #endif

    digitalWrite(OUTPUT_LED_PIN, msg & MSG_ON_BIT);
    delay(LOOP_DELAY);
}

#endif