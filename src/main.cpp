// F.R.I.E.N.D.S - Clapper

#include<Arduino.h>
//#define DEBUG

// Consts
const byte  PIN_SENSOR             = A0;   // Sound Sensor Pin
const int   PIN_OUTPUT             = 12;   // Output Pin
const int   CLAP_INTERVAL          = 500;  // Listening Delay
const int   CLAP_MIN               = 100;  // Hysteresis lower limit
const int   CLAP_MAX               = 220;  // Hysteresis higher limit
const byte  NR_CLAP_MIN            = 4;    // Min Claps to consider
const byte  NR_CLAP_MAX            = 4;    // Max Claps to consider
const byte  TRIGGER_CLAPS          = 4;    // Number of Claps to trigger action
const int   DELAY_BETWEEN_CLAPS    = 40;   // Delay between each claps to avoid noise

// Function Signatures
byte DetectClaps(byte pin, byte min, byte max);

// Variables
bool ledValue = LOW;

// Program
void setup() {
#ifdef DEBUG
  Serial.begin(9600);
  Serial.println("F.R.I.E.N.D.S - Clapper");
#endif
  pinMode(PIN_OUTPUT, OUTPUT);
  digitalWrite(PIN_OUTPUT, ledValue);
  ledValue = HIGH;
}

void loop() 
{
  int clapsDetected = DetectClaps(PIN_SENSOR, NR_CLAP_MIN, NR_CLAP_MAX);

  if (clapsDetected == TRIGGER_CLAPS)
  {
    digitalWrite(PIN_OUTPUT, ledValue);
    ledValue =  (ledValue == HIGH) ? LOW : HIGH;
  }

#ifdef DEBUG
  Serial.print("Claps detected: ");
  Serial.println(clapsDetected);
#endif  
}

byte DetectClaps(byte pin, byte min, byte max) 
{
  int currentSound;
  byte claps = 0;
  bool clapped = false;
  unsigned long time = millis();

  while (millis() - time < CLAP_INTERVAL && (claps < max || claps < min)) 
  {
    // The analog sensor returns an average of 512 (2.5V)
    // When a clap (or any higher noise) is captured by the sensor it will have spikes
    // These spikes can vary from 0 to 1023, outside the Hysteresis range
    currentSound = 512 - analogRead(pin);
    currentSound = abs(currentSound);

    // Schmitt Trigger
    // More info on: https://howtomechatronics.com/how-it-works/electrical-engineering/schmitt-trigger/
    if (currentSound > CLAP_MAX && !clapped) 
    {
      clapped = true;
      claps++;
      delay(DELAY_BETWEEN_CLAPS);
      time = millis();
    } 
    else if (currentSound < CLAP_MIN && clapped) 
    {
      clapped = false;
    }
  }

  return claps;
}
