#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

// ADXL345 Instanz
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

// Parameter für Bewegungserkennung
float motionThreshold = 1.5;   // anpassen (m/s^2 Differenz)
float smoothingFactor = 0.9;   // Low-pass filter

float prevMagnitude = 10;
float smoothMagnitude = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!accel.begin()) {
    Serial.println("Kein ADXL345 gefunden!");
    while (1);
  }

  accel.setRange(ADXL345_RANGE_4_G);
  Serial.println("ADXL345 bereit.");
}

void loop() {
  sensors_event_t event;
  accel.getEvent(&event);

  // Rohwerte
  float x = event.acceleration.x;
  float y = event.acceleration.y;
  float z = event.acceleration.z;

  // Betrag der Beschleunigung berechnen
  float magnitude = sqrt(x*x + y*y + z*z);

  // Low-pass Filter (glätten)
  smoothMagnitude = smoothingFactor * smoothMagnitude + (1 - smoothingFactor) * magnitude;

  // Änderung berechnen
  float delta = abs(smoothMagnitude - prevMagnitude);

  prevMagnitude = smoothMagnitude;

  // Bewegung erkennen
  bool tooMuchMotion = delta > motionThreshold;

  // Ausgabe
  Serial.print("X: "); Serial.print(x);
  Serial.print(" Y: "); Serial.print(y);
  Serial.print(" Z: "); Serial.print(z);
  Serial.print(" | Δ: "); Serial.print(delta);

  if (tooMuchMotion) {
    Serial.println(" -> BEWEGUNG ZU STARK (Daten verwerfen)");
  } else {
    Serial.println(" -> RUHIG (Daten OK)");
  }

  delay(100);
}