 #define BLYNK_TEMPLATE_ID "TMPL3u1QcA516"
#define BLYNK_TEMPLATE_NAME "Water Level"
#define BLYNK_AUTH_TOKEN "KYfvqqYg61ypOsURR5xrnKHSr7GdulGA"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char ssid[] = "poco X3";
char pass[] = "asdf1234";

int emptyTankDistance = 15;
int fullTankDistance = 3;
int triggerPer = 20;

#define TRIG 12 // D6
#define ECHO 13 // D7
#define Relay 14 // D5

#define V_LEVEL V1
#define V_MODE V3
#define V_RELAY V4

float duration;
float distance;
int waterLevelPer;

bool relayState = false;
bool autoMode = true;

BlynkTimer timer;

BLYNK_WRITE(V_MODE) {
  autoMode = param.asInt();
}

BLYNK_WRITE(V_RELAY) {
  if (!autoMode) {
    relayState = param.asInt();
    digitalWrite(Relay, relayState);
  }
}

void measureWaterLevel() {

  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH);
  distance = duration * 0.034 / 2;

  if (distance > fullTankDistance && distance < emptyTankDistance) {

    waterLevelPer = map((int)distance, emptyTankDistance, fullTankDistance, 0, 100);
    Blynk.virtualWrite(V_LEVEL, waterLevelPer);

    if (autoMode) {
      if (waterLevelPer < triggerPer) {
        digitalWrite(Relay, HIGH);
        relayState = true;
      }

      if (distance <= fullTankDistance) {
        digitalWrite(Relay, LOW);
        relayState = false;
      }

      Blynk.virtualWrite(V_RELAY, relayState);
    }
  }

  Serial.print("Water Level: ");
  Serial.print(waterLevelPer);
  Serial.println("%");
}

void setup() {
  Serial.begin(9600);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(Relay, OUTPUT);

  digitalWrite(Relay, LOW);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(1000L, measureWaterLevel);
}

void loop() {
  Blynk.run();
  timer.run();
}
