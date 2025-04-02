#include <Arduino.h>
#include <TimerOne.h>

#define BUTTON1 3
#define BUTTON2 2
#define TRIG_PIN 7
#define ECHO_PIN 6
#define LEDbutton1 12  
#define LEDbutton2 10  
#define LEDsensor 8   
#define LEDserial 5   
#define LEDtimer 4  

volatile bool button1Pressed = false;
volatile bool button2Pressed = false;
volatile bool serialReceived = false;
volatile bool timerTriggered = false;

unsigned long ledStartTime = 0;
int activeLED = -1;  

void button1ISR() { 
    button1Pressed = true; 
}
void button2ISR() { 
    button2Pressed = true; 
}
void timerISR() { 
    timerTriggered = true; 
}
void serialEvent() { 
    while (Serial.available()) {
        Serial.read();
    }  
    serialReceived = true; 
}

float getDistance() {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH);
    return (duration * 0.0343) / 2;
}

void resetLEDs() {
    digitalWrite(LEDbutton1, LOW);
    digitalWrite(LEDbutton2, LOW);
    digitalWrite(LEDsensor, LOW);
    digitalWrite(LEDserial, LOW);
    digitalWrite(LEDtimer, LOW);
    activeLED = -1;
}

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
