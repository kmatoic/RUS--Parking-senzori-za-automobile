/**
 * @file Kod.cpp
 * @brief Glavni program za upravljanje višestrukim prekidima i njihovim prioritetima
 */

#include <Arduino.h>
#include <TimerOne.h>

/** @def BUTTON1
 *  @brief Pin za prvi gumb (glavni) */
#define BUTTON1 3
/** @def BUTTON2
 *  @brief Pin za drugi gumb */
#define BUTTON2 2
/** @def TRIG_PIN
 *  @brief Pin za trig senzora udaljenosti */
#define TRIG_PIN 7
/** @def ECHO_PIN
 *  @brief Pin za echo senzora udaljenosti */
#define ECHO_PIN 6
/** @def LEDbutton1
 *  @brief Pin za LED vezan uz prvi gumb */
#define LEDbutton1 10
/** @def LEDbutton2
 *  @brief Pin za LED vezan uz drugi gumb */
#define LEDbutton2 9  
/** @def LEDsensor
 *  @brief Pin za LED vezan uz senzor udaljenosti */
#define LEDsensor 8   
/** @def LEDserial
 *  @brief Pin za LED vezan uz serijsku komunikaciju */
#define LEDserial 5   
/** @def LEDtimer
 *  @brief Pin za LED vezan uz timer */
#define LEDtimer 4  

/** @var volatile bool button1Pressed
 *  @brief Zastavica koja označava je li pritisnut prvi gumb */
volatile bool button1Pressed = false;
/** @var volatile bool button2Pressed
 *  @brief Zastavica koja označava je li pritisnut drugi gumb */
volatile bool button2Pressed = false;
/** @var volatile bool serialReceived
 *  @brief Zastavica koja označava je li primljena serijska komunikacija */
volatile bool serialReceived = false;
/** @var volatile bool timerTriggered
 *  @brief Zastavica koja označava je li timer istekao */
volatile bool timerTriggered = false;

/** @var unsigned long ledStartTime
 *  @brief Vrijeme kada je LED upaljen */
unsigned long ledStartTime = 0;
/** @var int activeLED
 *  @brief Trenutno aktivni LED (-1 ako nijedan nije aktivan) */
int activeLED = -1;  

/**
 * @brief Prekidna rutina za prvi gumb
 */
void button1ISR() { 
    button1Pressed = true; 
}

/**
 * @brief Prekidna rutina za drugi gumb
 */
void button2ISR() { 
    button2Pressed = true; 
}

/**
 * @brief Prekidna rutina za timer
 */
void timerISR() { 
    timerTriggered = true; 
}

/**
 * @brief Prekidna rutina za serijsku komunikaciju
 */
void serialEvent() { 
    while (Serial.available()) {
        Serial.read();
    }  
    serialReceived = true; 
}

/**
 * @brief Mjeri udaljenost pomoću senzora
 * @return Vraća izmjerenu udaljenost u centimetrima
 */
float getDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH);
    return (duration * 0.0343) / 2;
}

/**
 * @brief Gasí sve LED diode i resetira aktivni LED
 */
void resetLEDs() {
    digitalWrite(LEDbutton1, LOW);
    digitalWrite(LEDbutton2, LOW);
    digitalWrite(LEDsensor, LOW);
    digitalWrite(LEDserial, LOW);
    digitalWrite(LEDtimer, LOW);
    activeLED = -1;
}

/**
 * @brief Inicijalizacija hardvera i postavke početnih vrijednosti
 */
void setup() {
    Serial.begin(9600);
    pinMode(BUTTON1, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON1), button1ISR, FALLING);
    pinMode(BUTTON2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON2), button2ISR, FALLING);
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(LEDbutton1, OUTPUT);
    pinMode(LEDbutton2, OUTPUT);
    pinMode(LEDsensor, OUTPUT);
    pinMode(LEDserial, OUTPUT);
    pinMode(LEDtimer, OUTPUT);
    Timer1.initialize(1000000);
    Timer1.attachInterrupt(timerISR);
}

/**
 * @brief Glavna petlja programa koja upravlja LED-ovima na temelju prekida
 */
void loop() {
    unsigned long currentMillis = millis();

    if (activeLED != -1 && currentMillis - ledStartTime >= 2000) {
        resetLEDs();
    }
    else if (activeLED == -1) {
        if (button1Pressed) {
            Serial.println("Prekid - Glavni gumb pritisnut (najviši prioritet)");
            resetLEDs();
            digitalWrite(LEDbutton1, HIGH);
            activeLED = LEDbutton1;
            ledStartTime = currentMillis;
            button1Pressed = false;
        }
        else if (button2Pressed) {
            Serial.println("Prekid - Drugi gumb pritisnut");
            resetLEDs();
            digitalWrite(LEDbutton2, HIGH);
            activeLED = LEDbutton2;
            ledStartTime = currentMillis;
            button2Pressed = false;
        }
        else if (getDistance() < 50) {
            Serial.println("Prekid - Udaljenost manja od 50 cm");
            resetLEDs();
            digitalWrite(LEDsensor, HIGH);
            activeLED = LEDsensor;
            ledStartTime = currentMillis;
        }
        else if (serialReceived) {
            Serial.println("Prekid - Serijska komunikacija aktivna");
            resetLEDs();
            digitalWrite(LEDserial, HIGH);
            activeLED = LEDserial;
            ledStartTime = currentMillis;
            serialReceived = false;
        }
        else if (timerTriggered) {
            Serial.println("Prekid - Timer aktivan");
            resetLEDs();
            digitalWrite(LEDtimer, HIGH);
            activeLED = LEDtimer;
            ledStartTime = currentMillis;
            timerTriggered = false;
        }
    }
}
