/**
 * @file Kod.cpp
 * @brief Program za upravljanje LED diodom i sleep modom s buđenjem preko tipkala ili watchdog timera.
 */

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

const int LED_PIN = 13;       ///< Pin na kojem je spojena LED dioda.
const int BUTTON_PIN = 2;     ///< Pin na kojem je spojeno tipkalo.

volatile bool wakeUpByButton = false; ///< Zastavica za buđenje preko tipkala.
volatile bool wakeUpByTimer = false;  ///< Zastavica za buđenje preko watchdog timera.

/**
 * @brief Funkcija koja se poziva prilikom buđenja preko tipkala.
 * Postavlja zastavicu wakeUpByButton na true.
 */
void wakeUpButton() {
  wakeUpByButton = true;
}

/**
 * @brief Interrupt servisna rutina (ISR) za watchdog timer.
 * Postavlja zastavicu wakeUpByTimer na true prilikom buđenja.
 */
ISR(WDT_vect) {
  wakeUpByTimer = true;
}

/**
 * @brief Postavlja watchdog timer za buđenje iz sleep moda.
 * Konfigurira watchdog timer da generira interrupt nakon određenog vremena.
 */
void setupWatchdog() {
  cli();
  wdt_reset();
  WDTCSR |= (1 << WDCE) | (1 << WDE);
  WDTCSR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0);
  sei();
}

/**
 * @brief Uspava mikrokontroler u SLEEP_MODE_PWR_DOWN.
 * Mikrokontroler se može probuditi preko tipkala ili watchdog timera.
 */
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

/**
 * @brief Inicijalizacija pinova, serijske komunikacije i interrupta.
 */
void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.begin(9600);
  delay(200);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), wakeUpButton, FALLING);

  Serial.println("Početak programa");
}

/**
 * @brief Glavna petlja programa.
 * Uključuje LED diodu na 5 sekundi, zatim ulazi u sleep mode.
 * Nakon buđenja, ispisuje uzrok buđenja.
 */
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