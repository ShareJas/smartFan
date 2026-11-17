#include <Arduino.h>

const int BTN = 6;
const int PWM_PIN = 16;
const int PWM_CH = 0;
const int FREQ = 20000;   // 20 kHz, no whine
const int RES = 8;        // duty 0-255
const int unusedPins[] = {4, 5, 7, 15, 17, 18, 8, 9, 10, 11, 12, 13, 14, 21, 38, 39, 40, 41, 42};


bool running = false;
int duty = 0;


void setup() {
  Serial.begin(115200);
  pinMode(BTN, INPUT_PULLDOWN);       // button to 3.3V, press = HIGH
  
  for (int pin : unusedPins) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }
  ledcSetup(PWM_CH, FREQ, RES);
  ledcAttachPin(PWM_PIN, PWM_CH);
  ledcWrite(PWM_CH, 0);
}

void loop() {
  static bool last = LOW;
  bool now = digitalRead(BTN);
  if (now == HIGH && last == LOW) {   // rising edge = press
    running = !running;
    Serial.println(running ? "START" : "STOP");
    delay(30);                         // debounce
  }
  last = now;

  int target = running ? 180 : 0;      // later: map heart rate to duty
  if (duty < target) duty++;
  else if (duty > target) duty--;      // soft ramp
  ledcWrite(PWM_CH, duty);
  delay(5);
}