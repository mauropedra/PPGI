#include <Arduino.h>
#include <Esp.h>

// PUMP
#define PUMP 5 //D1
const long pump_time = 1000 * 60 * 60 * 12;
const long pump_duration = 1000 * 5;

// WATER LEVEL
#define LEVEL  4 //D2

// WIFI
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>
const char* ssid = "labnet (The Lab) ";
const char* password = "l4bn3t00";

// LIGHT
#define LIGHT A0 //A0
#define LIGHT_POWER 12 //D6
const long light_time = 1000;
int l;
int light;

// TEMPERATURE
#define TEMP 0 //D3
#define ONE_WIRE_BUS 2 // D4
#include <OneWire.h>
#include <DallasTemperature.h>
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
float temp;

// TIMER
#include <SimpleTimer.h>
SimpleTimer timer;

void setup() {
  Serial.begin(115200);
  pinMode(PUMP, OUTPUT);
  digitalWrite(PUMP, LOW);
  timer.setInterval(light_time, adjust_light);
  timer.setInterval(pump_time, pump_on);
  yield();
  Serial.println("Setup concluido");
}

void loop() {
  timer.run();
}

void connect_wifi() {
  WiFi.mode(WIFI_STA);
  delay(100);
  WiFi.begin(ssid, password);
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    retries++;
    if (retries > 10) {
      Serial.println("sem conexao");
      yield();
    }
  }
  delay(100);
  Serial.print("IP obtido: ");
  Serial.println(WiFi.localIP());
}

// Pin Descrição: Brown (VCC) ligado à alimentação positiva, amarelo (OUT) é a saída do sinal, azul (GND) ligado ao negativo da
// fonte de alimentação. A linha preta é uma seleção de nível válida, se estiver conectada a alta ou flutuante, o nível ativo é
// alto. Caso contrário, o nível ativo está baixo.

bool read_water_level() {
  if (digitalRead(LEVEL)) {
    return 1;
  } else {
    return 0;
  }
}


void read_water_temperature() {
  DS18B20.requestTemperatures();
  delay(100);
  temp = DS18B20.getTempCByIndex(0);
  delay(100);
  Serial.print("Temperature: ");
  Serial.println(temp);
}



// Turn the pump on for pump_duration milisecs
void pump_on() {
  if (read_water_level()) {
    Serial.println("ligando bomba");
    digitalWrite(PUMP, HIGH);
    delay(pump_duration);
    digitalWrite(PUMP, LOW);
    Serial.println("bomba desligada");
  } else {
    Serial.println("Falha ao ligar bomba: Falta agua");
  }
}

int read_light() {
  l =  analogRead(LIGHT);
  Serial.println(l);
  return l;
}

void adjust_light() {
  light = read_light();
  analogWrite(LIGHT_POWER, 1023 - light);
}

void send_data() {
}

void receive_data() {
}




