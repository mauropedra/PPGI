#include <Arduino.h>
#include <Esp.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>

// WIFI
const char* ssid = "labnet (The Lab) ";
const char* password = "l4bn3t00";
const char* ssid2 = "hsNCE";
const char* password2 = NULL;

// THINGSPEAK
WiFiClient  client_thingspeak;
String      apiKey            = "Y8VNVETDNZQ4EBRA";
const char* server_thingspeak = "api.thingspeak.com";
const long  thingspeak_time   = 16000;


// PUMP laranja
#define PUMP_LARANJEIRA 5
const long pump_time = 1000 * 60 * 60 * 12;
const long pump_duration = 1000 * 4;


// WATER LEVEL
#define LEVEL  D2

// LIGHT
#define LIGHT A0 //A0
#define LIGHT_POWER 12 //D6
const long light_time = 1000;
int l;
int light;

// TEMPERATURE
#define TEMP 0 //D3
#define ONE_WIRE_BUS D4
#include <OneWire.h>
#include <DallasTemperature.h>
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
float temp;

// TIMER
#include <SimpleTimer.h>
SimpleTimer timer;



//Serial.println("Tentando conectar");
//int retries = 0;
//while (WiFi.status() != WL_CONNECTED) {
//  client_thingspeak.stop();
//  WiFi.mode(WIFI_STA);
//  delay(100);
//  if (retries > 20) {
//    Serial.println("sem conexao");
//    yield();
//    ESP.reset();
//  }
//  if (retries % 3 == 0) {
//    WiFi.begin(ssid, password);
//  }
//  if (retries % 3 == 1) {
//    WiFi.begin(ssid2);
//  } else {
//    WiFi.begin("lfkopp", "dezembro");
//  }
//  delay(500);
//  Serial.print(".");
//  retries++;
//
//  delay(2000);
//}
//delay(100);
//Serial.print("IP obtido: ");
//Serial.println(WiFi.localIP());
//}

void setup() {
  Serial.begin(115200);
  pinMode(ONE_WIRE_BUS, INPUT);
  connect_wifi(ssid, password);
  timer.setInterval(light_time,      adjust_light);
  timer.setInterval(pump_time,       pump_on);
  timer.setInterval(thingspeak_time, read_water_temperature);
  timer.setInterval(1000 * 60 * 15, conecta);
  Serial.println("Setup concluido");
}

void loop() {
  timer.run();
  yield();
}


void conecta()
{
  connect_wifi(ssid, password);
  connect_wifi(ssid2, password2);
}


void connect_wifi(const char*  s, const char* p) {
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.print("Connecting to: ");
    Serial.println(s);
    WiFi.begin((const char*)s, (const char*)p);
    int retries = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
      retries++;
      delay(500);
      Serial.print(".");
      if (retries > 20)
      {
        break;
      }
    }

    Serial.println("WiFi connected");
    Serial.print("SSID: ");
    Serial.println(s);
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

  }
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


void thing(float temp) {
  String postStr = apiKey;
  postStr += "&field1=";
  postStr += String(temp);
  postStr += "\r\n\r\n";

  if (client_thingspeak.connect(server_thingspeak, 80)) {
    client_thingspeak.print("POST /update HTTP/1.1\n");
    client_thingspeak.print("Host: api.thingspeak.com\n");
    client_thingspeak.print("Connection: close\n");
    client_thingspeak.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client_thingspeak.print("Content-Type: application/x-www-form-urlencoded\n");
    client_thingspeak.print("Content-Length: ");
    client_thingspeak.print(postStr.length());
    client_thingspeak.print("\n\n");
    client_thingspeak.print(postStr);
    Serial.println(postStr);
  } else {
    Serial.println("reconectando");
    connect_wifi(ssid, password);
  }
}

void read_water_temperature() {
  DS18B20.requestTemperatures();
  yield();
  delay(100);
  temp = DS18B20.getTempCByIndex(0);
  Serial.print("Temperature: ");
  Serial.println(temp);
  thing(temp);
}

// Turn the pump on for pump_duration milisecs
void pump_on() {
  //if (read_water_level()) {
  Serial.println("ligando bomba");
  digitalWrite(PUMP_LARANJEIRA, HIGH);
  delay(pump_duration);
  digitalWrite(PUMP_LARANJEIRA, LOW);
  Serial.println("bomba desligada");
  // } else {
  //   Serial.println("Falha ao ligar bomba: Falta agua");
  //}
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


