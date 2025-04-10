#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

const int ledPin = 13;       ///< Pin za LED diodu
const int buttonPin = 2;     ///< Pin za tipkalo

volatile bool wakeUpByButton = false; ///< Zastavica za buđenje tipkalom
volatile bool wakeUpByWDT = false;    ///< Zastavica za buđenje watchdog timerom

/**
 * @brief Interrupt rutina za buđenje tipkalom
 * 
 * Postavlja zastavicu wakeUpByButton na true kada se aktivira interrupt s tipkala
 */
void wakeUpInterrupt() {
  wakeUpByButton = true;
}

/**
 * @brief Interrupt rutina za watchdog timer (WDT)
 * 
 * Postavlja zastavicu wakeUpByWDT na true kada WDT istekne
 */
ISR(WDT_vect) {
  wakeUpByWDT = true;
}

/**
 * @brief Postavljanje watchdog timera
 * 
 * Konfigurira WDT za ~8s timeout s interruptom (bez resetiranja)
 */
void setupWatchdogTimer() {
  cli();
  wdt_reset();
  WDTCSR |= (1 << WDCE) | (1 << WDE);  
  WDTCSR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0); 
  sei();
}

/**
 * @brief Ulazak u sleep mode
 * 
 * Postavlja mikroprocesor u najdublji sleep mode (PWR_DOWN) i čeka na buđenje
 * Buđenje je moguće ili preko tipkala ili watchdog timera
 */
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

/**
 * @brief Inicijalizacija sustava
 * 
 * Postavlja pinove, serijsku komunikaciju i interrupt za tipkalo
 */
void setup() {
  Serial.begin(9600);
  delay(200);

  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(buttonPin), wakeUpInterrupt, FALLING);

  Serial.println("Pokretanje sustava");
}

/**
 * @brief Glavna programska petlja
 * 
 * Uključuje LED na 5 sekundi, zatim ulazi u sleep mode.
 * Nakon buđenja, ispisuje izvor buđenja (tipkalo ili WDT)
 */
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