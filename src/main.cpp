#define BLYNK_TEMPLATE_ID "TMPL6CelwIvMc"
#define BLYNK_TEMPLATE_NAME "Latihan Responsi Praktiku Internet Of Things"
#define BLYNK_AUTH_TOKEN "o_gOTD-BzXaQ5-8ABqsTCg3G0SqcaBRp"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

const int buttonPin = 27;
bool lastButtonState = HIGH;
const int ledPin = 26;
bool ledState = false;
const int trigPin = 5;
const int echoPin = 18;

BlynkTimer timer;

LiquidCrystal_I2C lcd(0x27, 16, 2);

float getDistance()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;
  return distance;
}

void writeLcd(float distance, const char *conditionText, const char *statusText)
{
  lcd.setCursor(0, 0);
  lcd.print("Jarak: ");
  lcd.println(distance, 1);

  lcd.setCursor(0, 1);
  lcd.print("K: ");
  lcd.print(conditionText);

  lcd.setCursor(9, 1);
  lcd.print("S: ");
  lcd.print(statusText);
}

void sendDataToBlynk()
{
  float distance = getDistance();
  Serial.print("Jarak: ");
  Serial.print(distance);
  Serial.println(" cm");

  bool condition = distance > 20;
  const char *conditionText = condition ? "Aman " : "Dekat";

  bool status = digitalRead(ledPin);
  const char *statusText = status ? "ON" : "OFF";

  if (status)
  {
    writeLcd(distance, conditionText, statusText);
  }
  else
  {
    lcd.clear();
  }

  Blynk.virtualWrite(V0, distance);
  Blynk.virtualWrite(V1, status);
  Blynk.virtualWrite(V2, condition);
}

void setup()
{
  Serial.begin(115200);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);

  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ESP32 Ready!");

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Serial.println("Everything is ready!");
  timer.setInterval(1000, sendDataToBlynk);
}

void loop()
{
  Blynk.run();
  timer.run();

  int buttonState = digitalRead(buttonPin);

  if (lastButtonState == HIGH && buttonState == LOW)
  {
    ledState = !ledState;
    digitalWrite(ledPin, ledState);
    delay(50);
  }

  lastButtonState = buttonState;
}
