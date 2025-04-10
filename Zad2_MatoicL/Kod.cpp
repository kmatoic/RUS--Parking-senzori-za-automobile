#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

const int ledPin = 13;
const int buttonPin = 2;

volatile bool wakeUpByButton = false;
volatile bool wakeUpByWDT = false;

void wakeUpInterrupt() {
  wakeUpByButton = true;
}

ISR(WDT_vect) {
  wakeUpByWDT = true;
}

void setupWatchdogTimer() {
  cli();
  wdt_reset();
  WDTCSR |= (1 << WDCE) | (1 << WDE);  
  WDTCSR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0); 
  sei();
}

void enterSleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  setupWatchdogTimer();

  wakeUpByButton = false;
  wakeUpByWDT = false;

  noInterrupts();
  interrupts(); 

  sleep_cpu();

  while (!wakeUpByButton && !wakeUpByWDT){}

  sleep_disable();
}

void setup() {
  Serial.begin(9600);
  delay(200);

  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(buttonPin), wakeUpInterrupt, FALLING);

  Serial.println("Pokretanje sustava");
}

void loop() {
  digitalWrite(ledPin, HIGH);
  Serial.println("LED uključen (5s)");
  delay(5000);
  digitalWrite(ledPin, LOW);
  Serial.println("LED isključen. Ulazak u sleep mode.");

  enterSleep();

  if (wakeUpByButton) {
    Serial.println("Buđenje: Eksterni prekid (tipkalo)");
  } else if (wakeUpByWDT) {
    Serial.println("Buđenje: Watchdog timer (~8s timeout)");
  } else {
    Serial.println("Buđenje: Nepoznat izvor");
  }

  delay(500);
}
