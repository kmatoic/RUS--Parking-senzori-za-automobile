#include <TimerOne.h>

/** @file Kod.cpp
 *  @brief Glavni program za upravljanje višestrukim prekidima i njihovim prioritetima.
 *  
 *  Program koristi prekide za detekciju pritiska tipki, promjene potenciometra,
 *  promjene osvjetljenja, serijske komunikacije i timer za upravljanje LED diodama.
 */

const int button = 2;          ///< Pin za prvo tipkalo
const int button2 = 3;         ///< Pin za drugo tipkalo
const int ledButton = 4;       ///< Pin za LED vezanu uz prvo tipkalo
const int ledButton2 = 5;      ///< Pin za LED vezanu uz drugo tipkalo
const int ledPotentiometer = 6;///< Pin za LED vezanu uz potenciometar
const int ledLightSensor = 7;  ///< Pin za LED vezanu uz senzor svjetlosti
const int ledSerial = 8;       ///< Pin za LED vezanu uz serijsku komunikaciju
const int ledTimer = 9;        ///< Pin za LED vezanu uz timer
const int lightSensor = A0;    ///< Analogni pin za senzor svjetlosti
const int potentiometer = A1;  ///< Analogni pin za potenciometar
const float GAMMA = 0.7;       ///< Gama vrijednost za izračun svjetlosti
const float RL10 = 50;         ///< Referentni otpor za izračun svjetlosti

/// Zastavice za prekide
volatile bool timerFlag = false;          ///< Zastavica za timer prekid
volatile bool serialFlag = false;        ///< Zastavica za serijski prekid
volatile bool lightSensorFlag = false;   ///< Zastavica za prekid senzora svjetlosti
volatile bool potentiometerFlag = false; ///< Zastavica za prekid potenciometra
volatile bool buttonFlag = false;        ///< Zastavica za prekid prvog tipkala
volatile bool buttonFlag2 = false;       ///< Zastavica za prekid drugog tipkala

unsigned long previousMillis = 0;        ///< Vrijeme zadnjeg ažuriranja
const unsigned long interval = 2000;     ///< Interval trajanja LED upaljene (2 sekunde)

/**
 * @brief Prekidna rutina za prvo tipkalo
 * 
 * Postavlja zastavicu buttonFlag na true kada je tipkalo pritisnuto.
 */
void isr_button() {
    buttonFlag = true;
}

/**
 * @brief Prekidna rutina za drugo tipkalo
 * 
 * Postavlja zastavicu buttonFlag2 na true kada je tipkalo pritisnuto.
 */
void isr_button2() {
    buttonFlag2 = true;
}

/**
 * @brief Prekidna rutina za timer
 * 
 * Postavlja zastavicu timerFlag na true svake sekunde.
 */
void timerISR() {
    timerFlag = true;
}

/**
 * @brief Prekidna rutina za serijsku komunikaciju
 * 
 * Postavlja zastavicu serialFlag na true kada se primi podatak preko serijske veze.
 */
void serialEvent() {
    while (Serial.available()) {
        Serial.read();
    }
    serialFlag = true;
}

/**
 * @brief Čitanje vrijednosti potenciometra
 * 
 * Očitava vrijednost potenciometra i postavlja zastavicu ako je vrijednost veća od 512.
 */
void getPotValue() {
    int potValue = analogRead(potentiometer);
    if (potValue > 512) potentiometerFlag = true;
}

/**
 * @brief Mjerenje intenziteta svjetlosti
 * 
 * Računa intenzitet svjetlosti u luksima i postavlja zastavicu ako je vrijednost manja od 300.
 */
void getLight() {
    int analogValue = analogRead(A0);
    float voltage = analogValue / 1024. * 5;
    float resistance = 2000 * voltage / (1 - voltage / 5);
    float lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));
    if (lux < 300) lightSensorFlag = true;
}

/**
 * @brief Inicijalizacija hardvera
 * 
 * Postavlja pinove kao ulaze/izlaze, inicijalizira serijsku komunikaciju,
 * postavlja prekidne rutine za tipkala i timer.
 */
void setup() {
    pinMode(button, INPUT_PULLUP);
    pinMode(button2, INPUT_PULLUP);
    pinMode(ledTimer, OUTPUT);
    pinMode(ledSerial, OUTPUT);
    pinMode(ledButton, OUTPUT);
    pinMode(ledButton2, OUTPUT);
    pinMode(ledLightSensor, OUTPUT);
    pinMode(ledPotentiometer, OUTPUT);
    pinMode(lightSensor, INPUT);
    pinMode(potentiometer, INPUT);
    Serial.begin(9600);
    delay(100);
    while (Serial.available()) {
        Serial.read();
    }

    attachInterrupt(digitalPinToInterrupt(button), isr_button, FALLING);
    attachInterrupt(digitalPinToInterrupt(button2), isr_button2, FALLING);
    Timer1.initialize(1000000);
    Timer1.attachInterrupt(timerISR);
}

/**
 * @brief Glavna petlja programa
 * 
 * Provjerava zastavice prekida i upravlja LED diodama prema prioritetu.
 * LED dioda ostaje upaljena 2 sekunde nakon aktivacije.
 */
void loop() {
    static unsigned long ledStartTime = 0; ///< Vrijeme kada je LED upaljena
    static int activeLed = -1;            ///< Trenutno aktivna LED (-1 znači nijedna)
    
    getPotValue();
    getLight();
    
    if (activeLed == -1) {
        if (buttonFlag) {
            Serial.println("Prekid - tipkalo pritisnuto (najviši prioritet)!");
            activeLed = ledButton;
            buttonFlag = false;
        } else if (buttonFlag2) {
            Serial.println("Prekid - tipkalo 2 pritisnuto!");
            activeLed = ledButton2;
            buttonFlag2 = false;
        } else if (potentiometerFlag) {
            Serial.println("Prekid - potenciometar ima veću vrijednost od 512!");
            activeLed = ledPotentiometer;
            potentiometerFlag = false;
        } else if (lightSensorFlag) {
            Serial.println("Prekid - osvjetljenje je manje od 300 luksa!");
            activeLed = ledLightSensor;
            lightSensorFlag = false;
        } else if (serialFlag) {
            Serial.println("Prekid - serijski ulaz!");
            activeLed = ledSerial;
            serialFlag = false;
        } else if (timerFlag) {
            Serial.println("Prekid - timer!");
            activeLed = ledTimer;
            timerFlag = false;
        }

        if (activeLed != -1) {
            digitalWrite(activeLed, HIGH);
            ledStartTime = millis();
        }
    }
    
    if (activeLed != -1 && millis() - ledStartTime >= interval) {
        digitalWrite(activeLed, LOW);
        activeLed = -1;
    }
}
