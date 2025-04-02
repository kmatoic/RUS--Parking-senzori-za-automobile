#include <TimerOne.h>

const int button = 2;
const int button2 = 3;
const int ledButton = 4;
const int ledButton2 = 5;
const int ledPotentiometer = 6;
const int ledLightSensor = 7;
const int ledSerial = 8;
const int ledTimer = 9;
const int lightSensor = A0;
const int potentiometer = A1;
const float GAMMA = 0.7;
const float RL10 = 50;

volatile bool timerFlag = false;
volatile bool serialFlag = false;
volatile bool lightSensorFlag = false;
volatile bool potentiometerFlag = false;
volatile bool buttonFlag = false;
volatile bool buttonFlag2 = false;

unsigned long previousMillis = 0;
const unsigned long interval = 2000;

void isr_button() {
    buttonFlag = true;
}

void isr_button2() {
    buttonFlag2 = true;
}

void timerISR() {
    timerFlag = true;
}

void serialEvent() {
    while (Serial.available()) {
        Serial.read();
    }
    serialFlag = true;
}

void getPotValue() {
    int potValue = analogRead(potentiometer);
    if (potValue > 512) potentiometerFlag = true;
}

void getLight() {
    int analogValue = analogRead(A0);
    float voltage = analogValue / 1024. * 5;
    float resistance = 2000 * voltage / (1 - voltage / 5);
    float lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));
    if (lux < 300) lightSensorFlag = true;
}

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

    attachInterrupt(digitalPinToInterrupt(button), isr_button, FALLING);
    attachInterrupt(digitalPinToInterrupt(button2), isr_button2, FALLING);
    Timer1.initialize(1000000);
    Timer1.attachInterrupt(timerISR);
}

void loop() {
    static unsigned long ledStartTime = 0;
    static int activeLed = -1;
    
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
