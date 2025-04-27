/**
 * @file sketch.ino
 * @brief Glavna datoteka za projekt senzorskog sustava s servo motorom i LCD-om.
 * @details Sustav koristi tri ultrazvučna senzora za detekciju prepreka, 
 * upravlja servo motorom, LED indikatorima i LCD zaslonom. Omogućuje prijelaz
 * u sleep mode radi uštede energije.
 */

#include <Wire.h>               ///< Biblioteka za I2C komunikaciju
#include <Servo.h>              ///< Biblioteka za upravljanje servo motorom
#include <LiquidCrystal_I2C.h>  ///< Biblioteka za LCD zaslon s I2C sučeljem
#include <avr/sleep.h>          ///< Biblioteka za upravljanje sleep modeovima
#include <avr/interrupt.h>      ///< Biblioteka za upravljanje prekidima

/**
 * @brief Inicijalizacija LCD zaslona s I2C sučeljem
 * @details Adresa I2C sučelja je 0x27, zaslon ima 20 stupaca i 4 retka
 */
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Definicije pinova za ultrazvučne senzore
#define trigPin1 3   ///< Trigger pin za desni ultrazvučni senzor (HC-SR04)
#define echoPin1 12  ///< Echo pin za desni ultrazvučni senzor (HC-SR04)
#define trigPin2 5   ///< Trigger pin za srednji ultrazvučni senzor (HC-SR04)
#define echoPin2 4   ///< Echo pin za srednji ultrazvučni senzor (HC-SR04)
#define trigPin3 7   ///< Trigger pin za lijevi ultrazvučni senzor (HC-SR04)
#define echoPin3 6   ///< Echo pin za lijevi ultrazvučni senzor (HC-SR04)

// Definicije pinova za ostale komponente
#define buzzer 8      ///< Pin za aktivni buzzer
#define switchPin 2   ///< Pin za slider za aktiviranje sleep modea (s PULLUP)
#define ledYellowL 13 ///< Pin za žutu LED diodu na lijevoj strani
#define ledOrangeL 11 ///< Pin za narančastu LED diodu na lijevoj strani
#define ledRedL 9     ///< Pin za crvenu LED diodu na lijevoj strani
#define ledRedR A2    ///< Pin za crvenu LED diodu na desnoj strani
#define ledOrangeR A1 ///< Pin za narančastu LED diodu na desnoj strani
#define ledYellowR A0 ///< Pin za žutu LED diodu na desnoj strani

Servo servoMotor; ///< Objekt za upravljanje servo motorom (standardni library)

/**
 * @brief Zastavica za indikaciju buđenja iz sleep modea
 * @details Postavlja se na true u prekidnoj rutini wakeUp()
 * @note Moraju biti volatile zbog optimizacije kompajlera
 */
volatile bool shouldWakeUp = false;

/**
 * @brief Prekidna rutina za buđenje iz sleep modea
 * @details Poziva se kada se detektira FALLING edge na switchPin (slider u stanju LOW)
 * @note Mora biti kratka i ne smije koristiti delay()
 */
void wakeUp() {
  shouldWakeUp = true;
}

/**
 * @brief Mjeri udaljenost pomoću HC-SR04 ultrazvučnog senzora
 * @param trigPin Pin za trigger signal (OUTPUT)
 * @param echoPin Pin za echo signal (INPUT)
 * @return Udaljenost u centimetrima
 * @details Funkcija šalje 10μs impuls na trigger pin, zatim mjeri širinu
 *          pulseIn() signala na echo pinu i pretvara u centimetre
 * @note Brzina zvuka je 343 m/s (0.034 cm/μs), dijelimo s 2 jer je signal putovao do objekta i natrag
 */
long measureDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2); // Kratka pauza za stabilizaciju
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10); // 10μs trigger impuls (HC-SR04 zahtjev)
  digitalWrite(trigPin, LOW);
  return pulseIn(echoPin, HIGH) * 0.034 / 2; // Pretvorba u cm
}

/**
 * @brief Ažurira stanje LED indikatora na temelju udaljenosti
 * @param distance Udaljenost najbliže prepreke u centimetrima
 * @details Funkcija pali LEDice prema sljedećim pravilima:
 *          - Žute LEDice < 50 cm
 *          - Narančaste LEDice < 30 cm
 *          - Crvene LEDice <= 10 cm
 * @note LEDice su organizirane u parove (lijevo/desno) za vizualnu simetriju
 */
void updateLeds(int distance) {
  digitalWrite(ledYellowL, distance < 50 ? HIGH : LOW);
  digitalWrite(ledYellowR, distance < 50 ? HIGH : LOW);
  digitalWrite(ledOrangeL, distance < 30 ? HIGH : LOW);
  digitalWrite(ledOrangeR, distance < 30 ? HIGH : LOW);
  digitalWrite(ledRedL, distance <= 10 ? HIGH : LOW);
  digitalWrite(ledRedR, distance <= 10 ? HIGH : LOW);
}

/**
 * @brief Isključuje sve komponente prije ulaska u sleep mode
 * @details Obavlja sljedeće radnje:
 *          - Zaustavlja buzzer
 *          - Briše LCD i isključuje backlight
 *          - Gasi sve LEDice
 *          - Odvaja servo motor
 */
void shutdownComponents() {
  noTone(buzzer);
  lcd.clear();
  lcd.noBacklight();
  updateLeds(1000); // Prosljeđivanje velike vrijednosti gasi sve LEDice
  servoMotor.detach();
}

/**
 * @brief Ulazi u sleep mode (PWR_DOWN)
 * @details Konfigurira najniži sleep mode gdje se zaustavlja sve osim
 *          vanjskih prekida. Budi se samo preko prekida sa slidera.
 * @note Prije ulaska u sleep mode moraju se isključiti sve komponente
 */
void enterSleep() {
  shutdownComponents();
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Najniža potrošnja energije
  sleep_enable();
  shouldWakeUp = false;

  noInterrupts();
  interrupts(); 
  sleep_cpu();
  
  // Čekanje na prekid za buđenje
  while (!shouldWakeUp){}
  
  sleep_disable();
}

/**
 * @brief Inicijalizacija hardvera i postavke pinova
 * @details Funkcija koja se izvodi jednom prilikom pokretanja:
 *          - Inicijalizacija serijske komunikacije
 *          - Postavljanje pinova za senzore
 *          - Postavljanje pinova za LEDice
 *          - Inicijalizacija servo motora
 *          - Inicijalizacija LCD zaslona
 *          - Postavljanje prekidne rutine za slider
 */
void setup() {
  Serial.begin(9600); // Inicijalizacija serijske komunikacije

  // Konfiguracija pinova za ultrazvučne senzore
  pinMode(trigPin1, OUTPUT); pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT); pinMode(echoPin2, INPUT);
  pinMode(trigPin3, OUTPUT); pinMode(echoPin3, INPUT);
  
  pinMode(buzzer, OUTPUT); // Konfiguracija pina za buzzer
  pinMode(switchPin, INPUT_PULLUP); // Slider s internim PULLUP otpornikom

  // Konfiguracija izlaznih pinova za LEDice
  pinMode(ledYellowL, OUTPUT);
  pinMode(ledOrangeL, OUTPUT);
  pinMode(ledRedL, OUTPUT);
  pinMode(ledYellowR, OUTPUT);
  pinMode(ledOrangeR, OUTPUT);
  pinMode(ledRedR, OUTPUT);

  servoMotor.attach(10); // Servo motor spojen na pin 10
  lcd.begin(20, 4); // Inicijalizacija LCD-a (20x4)
  lcd.backlight(); // Uključivanje pozadinskog osvjetljenja

  // Postavljanje prekidne rutine za tipku (FALLING edge - pomak slidera)
  attachInterrupt(digitalPinToInterrupt(switchPin), wakeUp, FALLING);
}

/**
 * @brief Glavna programska petlja
 * @details Kontinuirano obavlja sljedeće zadatke:
 *          - Provjerava stanje slidera za sleep mode
 *          - Mjeri udaljenost s tri senzora
 *          - Prikazuje podatke na LCD-u i Serial monitoru
 *          - Upravlja LED indikatorima i buzzerom
 *          - Upravlja servo motorom (osim kada je prepreka preblizu)
 */
void loop() {
  // Provjera tipke za sleep mode (HIGH)
  if (digitalRead(switchPin) == HIGH) {
    Serial.println("Ulazak u sleep mode...");
    enterSleep();
    return; // Prekida izvođenje loop() dok se ne probudi
  }

  // Ponovno povezivanje komponenti nakon buđenja
  servoMotor.attach(10);
  lcd.backlight();

  // Mjerenje udaljenosti sa tri senzora
  int dist1 = measureDistance(trigPin1, echoPin1); // Desni senzor
  int dist2 = measureDistance(trigPin2, echoPin2); // Srednji senzor
  int dist3 = measureDistance(trigPin3, echoPin3); // Lijevi senzor
  int minDist = min(dist1, min(dist2, dist3)); // Najbliža prepreka

  // Ispis udaljenosti na Serial monitor
  Serial.print("Lijevo: "); Serial.print(dist3);
  Serial.print(" cm | Sredina: "); Serial.print(dist2);
  Serial.print(" cm | Desno: "); Serial.print(dist1);
  Serial.println(" cm");

  // Prikaz udaljenosti na LCD-u
  lcd.setCursor(0, 0);
  lcd.print("L      S      D "); // Zaglavlje (Lijevo, Sredina, Desno)
  lcd.setCursor(0, 1);
  char buffer[21]; // Buffer za formatirani ispis (20 znakova + null terminator)
  snprintf(buffer, sizeof(buffer), "%-3d    %-3d    %-3d cm", dist3, dist2, dist1);
  lcd.print(buffer);

  // Prikaz upozorenja ako je prepreka preblizu
  if (minDist <= 10) {
    lcd.setCursor(0, 3);
    lcd.print("       STOP!!       ");
  } else {
    lcd.setCursor(0, 3);
    lcd.print("                    "); // Brisanje retka ako nema opasnosti
  }

  // Varijable za kontrolu servo motora
  static int pos = 0; // Trenutni položaj servo motora (0-180°)
  static bool goingForward = true; // Smjer kretanja servo motora

  // Ažuriranje LED indikatora
  updateLeds(minDist);

  // Logika upravljanja buzzerom i servo motorom
  if (minDist <= 10) {
    // Kritična udaljenost - kontinuirani zvučni signal i zaustavljanje motora
    tone(buzzer, 2000); // Visok ton kada je udaljenost manja od 10 cm
    servoMotor.detach(); // Zaustavljanje servo motora
  } else {
    if (minDist < 50) {
      // Upozorenje - isprekidani zvučni signal (frekvencija ovisi o udaljenosti)
      static unsigned long lastBeep = 0;
      static bool beepState = false;
      unsigned long now = millis();
      int beepDelay = map(minDist, 10, 50, 100, 600); // Dinamički delay

      if (now - lastBeep > beepDelay) {
        if (beepState) {
          noTone(buzzer);
        } else {
          tone(buzzer, 1000); // Srednji ton za upozorenje
        }
        beepState = !beepState;
        lastBeep = now;
      }
    } else {
      // Normalno stanje - bez zvučnog signala
      noTone(buzzer);
    }

    // Upravljanje servo motorom (oscilira između 0 i 180°)
    servoMotor.write(pos);
    delay(10); // Mala pauza za stabilizaciju

    // Promjena smjera na krajevima
    if (goingForward) {
      pos++;
      if (pos >= 180) goingForward = false;
    } else {
      pos--;
      if (pos <= 0) goingForward = true;
    }
  }
}