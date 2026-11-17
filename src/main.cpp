#include <Arduino.h>
//#define PULSESHIELD
//#define SERIALDEBUG

const uint8_t  BTN = 6;
const uint8_t  PWM_PIN = 16;
const uint8_t  PWM_CH = 0;
const uint32_t FREQ = 20000;   // 20 kHz
const uint8_t  RES = 8;        // duty 0-255
const uint32_t RAMP_US = 5000; // ramp step every 5 ms
const uint32_t DEBOUNCE_MS = 200;

#ifdef PULSESHIELD
const int unusedPins[] = {4,5,7,15,17,18,8,9,10,11,12,13,14,21,38,39,40,41,42};
#endif

volatile bool running = false;
volatile uint8_t duty = 0;
volatile uint8_t targetDuty = 0;
volatile uint32_t lastPress = 0;
volatile bool btnEvent = false;
hw_timer_t* rampTimer = nullptr;

void IRAM_ATTR onButton() {
  uint32_t now = millis();
  if (now - lastPress > DEBOUNCE_MS) {
    running = !running;
    lastPress = now;
    btnEvent = true;
  }
}

void IRAM_ATTR onRamp() {
  if (duty < targetDuty) duty++;
  else if (duty > targetDuty) duty--;
  ledcWrite(PWM_CH, duty);
}

void setupPWM(uint8_t ch, uint8_t pin, uint32_t freq, uint8_t res) {
  ledcSetup(ch, freq, res);
  ledcAttachPin(pin, ch);
  ledcWrite(ch, 0);
}

void setup() {
  Serial.begin(115200);
  pinMode(BTN, INPUT_PULLDOWN);

  #ifdef PULSESHIELD
  for (int pin : unusedPins) { pinMode(pin, OUTPUT); digitalWrite(pin, LOW); }
  #endif

  setupPWM(PWM_CH, PWM_PIN, FREQ, RES);
  attachInterrupt(digitalPinToInterrupt(BTN), onButton, RISING);

  rampTimer = timerBegin(0, 80, true);           // 1 µs ticks
  timerAttachInterrupt(rampTimer, &onRamp, true);
  timerAlarmWrite(rampTimer, RAMP_US, true);
  timerAlarmEnable(rampTimer);
}

void loop() {
  if (btnEvent) {
    btnEvent = false;
    #ifdef SERIALDEBUG
    Serial.println(running ? "START" : "STOP");
    #endif
  }

  targetDuty = running ? 180 : 0;
  // BLE code goes here later, no delay() needed
}