//librerias
#include <LedControl.h>
#include <SoftwareSerial.h>
#include <RTClib.h>
//pins
const int ledPin = 13;
const int sensorPin = A0;
LedControl lc = LedControl(12, 11, 10, 1);
const int bombaPin = 5;
SoftwareSerial bluetooth(6, 7);  // RX, TX
//variables
int HistoryValue = 0;
DateTime now;
int seconds;
RTC_DS3231 rtc;
int id_Plant = 0;
String BT_Received = "";
int horas = 0;
int currentValue;

  bool hayAgua;

// MatrizLed
byte smiley[8] = {  // Definir el patrón de la carita feliz
  0b00000000,
  0b01100110,
  0b01100110,
  0b00000000,
  0b00000000,
  0b10011001,
  0b01100110,
  0b00000000
};

byte sadge[8] = {  // Definir el patrón de la carita triste
  0b00000000,
  0b00000000,
  0b01100110,
  0b00000000,
  0b00000000,
  0b00111100,
  0b01000010,
  0b00000000
};


void setup() {
  horas = 0;
  pinMode(bombaPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  rtc.begin();
  bluetooth.begin(4800);
}

void loop() {
  sensorAgua();
  evaluarVolumen();
  now = rtc.now();
  seconds = now.second();
  Serial.print(now.second(), DEC);
  Serial.println();


  if (bluetooth.available()) {
    BT_Received = bluetooth.readString();
    id_Plant = atoi(BT_Received.c_str());
    evaluarHoras(id_Plant);
    Serial.print("Valor recibido:");
    Serial.println(id_Plant);
  }
  delay(100);

  if (siHay(currentValue)) {
    if (horas != 0) {
      funcionRiego(horas);
    }
  }
}

//funcion de la matriz
void functionLed() {
  lc.shutdown(0, false);  // Enciende el display
  lc.setIntensity(0, 2);  // Ajusta el brillo (0-15 posibles)
  lc.clearDisplay(0);
}


//Funciones sensor de agua
void sensorAgua() {
  currentValue = analogRead(sensorPin);
  HistoryValue = currentValue;
}

void evaluarVolumen() {
  if (HistoryValue > 360) {
    functionLed();
    for (int i = 0; i < 8; i++) {
      lc.setRow(0, i, smiley[i]);
    }
    delay(100);
  } else if (HistoryValue <= 360) {
    functionLed();
    for (int i = 0; i < 8; i++) {
      lc.setRow(0, i, sadge[i]);
    }
    delay(100);
  }
}


void evaluarHoras(int idPlant) {
  switch (idPlant) {
    case 1:
      horas = 30;
      Serial.println("Se encendera cada " + String(horas) + " horas");
      break;
    case 2:
      horas = 20;
      Serial.println("Se encendera cada " + String(horas) + " horas");
      break;
    case 3:
      horas = 60;
      Serial.println("Se encendera cada " + String(horas) + " horas");
      break;
    case 4:
      horas = 0;
      break;
  }
}
//funcion riego
void funcionRiego(int tiempo) {
  if (seconds % tiempo == 0) {
    digitalWrite(ledPin, HIGH);
    digitalWrite(bombaPin, HIGH);
    delay(1000);
    digitalWrite(bombaPin, LOW);
    digitalWrite(ledPin, LOW);
    Serial.print("Se recibio:" + String(tiempo));
  }
}

//otra funcion mas, confiemos que funcione
bool siHay(int valorAgua) {
  if (valorAgua > 350) {
    hayAgua = true;
  } else {
    hayAgua = false;
  }
  return hayAgua;
}
