/****************************************
 * IoT Smart Classroom - Attendance System
 * NodeMCU + RFID + DHT11 + Buzzer
 * Blynk + Google Sheets + Notifications
 ****************************************/

#define BLYNK_TEMPLATE_ID   "TMPL6r2sKLuK4"
#define BLYNK_TEMPLATE_NAME "Attendance"
#define BLYNK_AUTH_TOKEN    "ALRBDASF8LDaTua2USUotwZfrXOn0Aa6"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <DHT.h>

// WiFi
char ssid[] = "suniarboy";
char pass[] = "12345678";

// Google Sheet URL
String googleScriptUrl =
"https://script.google.com/macros/s/AKfycbx7sFqVo6rapERVsYwC5wt6OHjsMGA6rDOYJFa2LSGKSCKLxd2BFy8kLiJlLYPRdjZo/exec";

// RFID
#define SS_PIN  D1
#define RST_PIN D2
MFRC522 mfrc522(SS_PIN, RST_PIN);

// DHT11
#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Buzzer
#define BUZZER D3

// Blynk Pins
#define VP_RFID_UID      V0
#define VP_RFID_STATUS   V1
#define VP_PERSON_NAME   V2
#define VP_TEMPERATURE   V3
#define VP_HUMIDITY      V4
#define VP_STUDENT_COUNT V5

// Alerts
float TEMP_LIMIT = 35;
float HUM_LIMIT  = 70;

int studentCount = 0;

unsigned long lastCardTime = 0;
const unsigned long cardCooldown = 2000;

unsigned long lastSensorRead = 0;
const unsigned long sensorInterval = 5000;

// Authorized Cards
struct AuthorizedCard {
  String uid;
  String name;
};

AuthorizedCard authorizedCards[] = {
  {"51 05 03 07", "White Card"},
  {"F9 E5 9F 94", "Shingara Singh"},
  {"39 4A A6 94", "Balram"}
};

int numCards = sizeof(authorizedCards) / sizeof(authorizedCards[0]);

//------------------------------------------------
// Buzzer

void beepOnce() {
  digitalWrite(BUZZER, HIGH);
  delay(200);
  digitalWrite(BUZZER, LOW);
}

void beepTwice() {
  for(int i=0;i<2;i++) {
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(BUZZER, LOW);
    delay(200);
  }
}

//------------------------------------------------

void setup() {

  Serial.begin(9600);

  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  WiFi.begin(ssid, pass);

  Serial.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");

  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();

  SPI.begin();
  mfrc522.PCD_Init();

  dht.begin();

  Serial.println("Smart Classroom Ready");
  Serial.println("Tap RFID Card");
}

//------------------------------------------------

void loop() {

  Blynk.run();
  readRFID();
  readDHT11();

}

//------------------------------------------------
// DHT11 Sensor

void readDHT11() {

  if (millis() - lastSensorRead < sensorInterval) return;
  lastSensorRead = millis();

  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT Error");
    return;
  }

  Serial.print("Temp: ");
  Serial.print(temp);
  Serial.print("C  Humidity: ");
  Serial.println(hum);

  Blynk.virtualWrite(VP_TEMPERATURE, temp);
  Blynk.virtualWrite(VP_HUMIDITY, hum);

  // Alerts
  if(temp > TEMP_LIMIT) {
    Blynk.logEvent("high_temp", "Temperature above limit!");
  }

  if(hum > HUM_LIMIT) {
    Blynk.logEvent("high_humidity", "Humidity above limit!");
  }
}

//------------------------------------------------
// Google Sheets

void sendToGoogleSheets(String uid, String name, String status) {

  if (WiFi.status() != WL_CONNECTED) return;

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient https;

  https.begin(client, googleScriptUrl);
  https.addHeader("Content-Type", "application/json");

  String payload =
  "{\"uid\":\"" + uid +
  "\",\"name\":\"" + name +
  "\",\"status\":\"" + status + "\"}";

  int httpCode = https.POST(payload);

  if (httpCode > 0) {

    String response = https.getString();

    Serial.print("Google Sheets Response: ");
    Serial.println(response);

  }

  https.end();
}

//------------------------------------------------
// RFID Reader

void readRFID() {

  if (millis() - lastCardTime < cardCooldown) return;

  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  lastCardTime = millis();

  String cardUID = "";

  for (byte i = 0; i < mfrc522.uid.size; i++) {

    if (mfrc522.uid.uidByte[i] < 0x10) cardUID += "0";

    cardUID += String(mfrc522.uid.uidByte[i], HEX);

    if (i < mfrc522.uid.size - 1) cardUID += " ";
  }

  cardUID.toUpperCase();

  Serial.print("Card UID: ");
  Serial.println(cardUID);

  Blynk.virtualWrite(VP_RFID_UID, cardUID);

  bool authorized = false;
  String personName = "";

  for (int i = 0; i < numCards; i++) {

    if (cardUID == authorizedCards[i].uid) {
      authorized = true;
      personName = authorizedCards[i].name;
      break;
    }
  }

  //------------------------------------------------

  if (authorized) {

    beepOnce();

    studentCount++;
    Blynk.virtualWrite(VP_STUDENT_COUNT, studentCount);

    Serial.println("Access GRANTED");

    Blynk.virtualWrite(VP_RFID_STATUS, "Access Granted");
    Blynk.virtualWrite(VP_PERSON_NAME, personName);

    // Notification
    Blynk.logEvent("attendance_log", personName + " entered classroom");

    sendToGoogleSheets(cardUID, personName, "Present");

  }

  else {

    beepTwice();

    Serial.println("Access DENIED");

    Blynk.virtualWrite(VP_RFID_STATUS, "Access Denied");
    Blynk.virtualWrite(VP_PERSON_NAME, "Unknown");

    // Alert
    Blynk.logEvent("invalid_card", "Unauthorized card scanned!");

    sendToGoogleSheets(cardUID, "Unknown", "Unauthorized");

  }

  mfrc522.PICC_HaltA();
}