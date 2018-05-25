//3v gnd d2pin
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// firebase
#define FIREBASE_HOST "yourfirebase.firebaseio.com"
#define FIREBASE_AUTH "yourfirebaseauth"

//wifi
#define WIFI_SSID "yourSSID"
#define WIFI_PASSWORD "yourSSIDPASS"

//dht
#define DHTPIN 14     // what digital pin the DHT22 is conected to
#define DHTTYPE DHT22   // there are multiple kinds of DHT sensors

//lcd
LiquidCrystal_I2C lcd(0x3F,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

DHT dht(DHTPIN, DHTTYPE);


void writeLcd(float t, float h){
  String sicaklik = "Sicaklik: ";
  sicaklik += String(t, 2);
  sicaklik += "C";

  String nem = "Nem: ";
  nem += String(h, 2);
  nem +="%";

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(sicaklik);
  lcd.setCursor(0, 1);
  lcd.print(nem);
}

void writeSerial(float t, float h){
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");
}

void setupWifi(){
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Wifi connecting...");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Wifi connecting...");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
}

void setupFireBase(){
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void setupLcd(){
  lcd.init();
  lcd.backlight();
  lcd.print("2017");
}

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(2000);

  // Wait for serial to initialize.
  while(!Serial) { }

  setupLcd();
  setupWifi();
  setupFireBase();


  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Device Started");
  lcd.setCursor(0,1);
  lcd.print("----------------------");
}
void loop() {

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t) ){
    Serial.println("Failed to read from DHT sensor!");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Fail DHT Sensor");
    return;
  }

  if(!Firebase.failed()){
    Firebase.setFloat("Temperature", t);
    Firebase.setFloat("Humidity", h);
  }

  writeSerial(t, h);
  writeLcd(t, h);

  delay(200);
}
