#include <Arduino.h>

// Безпечні піни для завантаження
#define TRIG_PIN D5   // GPIO14
#define ECHO_PIN D6   // GPIO12
#define BUZZER_PIN D7 // GPIO13

#define RED_LED_PIN D1   // Перенесли з D3 на D1 (GPIO5) — повністю безпечний!
#define GREEN_LED_PIN D4 // GPIO2
#define BLUE_LED_PIN D0  // GPIO16

unsigned long lastTime = 0;

void setup()
{
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Вимикаємо все на старті
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(BLUE_LED_PIN, LOW);
  noTone(BUZZER_PIN);

  Serial.println("\n--- Smart Rangefinder Started ---");
}

void loop()
{
  if (millis() - lastTime >= 300)
  {
    lastTime = millis();

    // Генерація ультразвукового імпульсу
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Зчитування тривалості (таймаут 30000 мкс ~ 5м)
    long duration = pulseIn(ECHO_PIN, HIGH, 30000);
    short distanceCm = duration * 0.0343 / 2;

    if (distanceCm <= 0 || distanceCm > 400)
    {
      Serial.println("Distance: Out of range / Error");

      digitalWrite(GREEN_LED_PIN, LOW);
      digitalWrite(RED_LED_PIN, LOW);
      digitalWrite(BLUE_LED_PIN, LOW);
      noTone(BUZZER_PIN);
    }
    else
    {
      Serial.print("Distance: ");
      Serial.print(distanceCm);
      Serial.println(" cm");

      // Зона 1: Більше 50 см — Безпечно (Зелений)
      if (distanceCm > 50)
      {
        digitalWrite(GREEN_LED_PIN, HIGH);
        digitalWrite(RED_LED_PIN, LOW);
        digitalWrite(BLUE_LED_PIN, LOW);
        noTone(BUZZER_PIN);
      }
      // Зона 2: Від 20 до 50 см — Попередження (Жовтий = R + G)
      else if (distanceCm >= 20 && distanceCm <= 50)
      {
        digitalWrite(GREEN_LED_PIN, HIGH);
        digitalWrite(RED_LED_PIN, HIGH);
        digitalWrite(BLUE_LED_PIN, LOW);
        noTone(BUZZER_PIN);
      }
      // Зона 3: Менше 20 см — Небезпека (Червоний + Звук)
      else
      {
        digitalWrite(GREEN_LED_PIN, LOW);
        digitalWrite(RED_LED_PIN, HIGH);
        digitalWrite(BLUE_LED_PIN, LOW);
        tone(BUZZER_PIN, 600); // Звук 600 Гц
      }
    }
  }

  yield();
}