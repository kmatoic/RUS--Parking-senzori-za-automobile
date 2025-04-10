#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

const int LED_PIN = 13;
const int BUTTON_PIN = 2;

volatile bool wakeUpByButton = false;
volatile bool wakeUpByTimer = false;

void wakeUpButton() {
  wakeUpByButton = true;
}

ISR(WDT_vect) {
  wakeUpByTimer = true;
}

void setupWatchdog() {
  cli();
  wdt_reset();
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  WDTCSR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0);
  sei();
}

void enterSleep() {
  wakeUpByButton = false;
  wakeUpByTimer = false;

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  setupWatchdog();

  noInterrupts();
  interrupts();

  sleep_cpu();
  while (!wakeUpByButton && !wakeUpByTimer){}
  sleep_disable();
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.begin(9600);
  delay(200);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), wakeUpButton, FALLING);

  Serial.println("Početak programa");
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  Serial.println("LED dioda svijetli (5s)");
  delay(5000);
  digitalWrite(LED_PIN, LOW);
  Serial.println("Ulazak u sleep mode!");

  enterSleep();

  if (wakeUpByButton) {
    Serial.println("Buđenje: Eksterni prekid (tipkalo).");
  } else if (wakeUpByTimer) {
    Serial.println("Buđenje: Watchdog timer.");
  } else {
    Serial.println("Buđenje: Nepoznat uzrok.");
  }

  delay(1000);
}
