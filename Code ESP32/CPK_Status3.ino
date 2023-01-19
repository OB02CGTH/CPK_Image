#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <analogWrite.h>

#define WIFI_SSID "KORSAK"
#define WIFI_PASSWORD "0994541990"

#define API_KEY "AIzaSyAxYNZR1me3TJ2u7V4mUctX_h2mUcS1-PQ"
#define DATABASE_URL "https://car-parking-system-71406-default-rtdb.asia-southeast1.firebasedatabase.app/"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int intStatus;
int intSet;
String stringStatus;
String path;
String pathset;
bool signupOK = false;
int rgb[2][3] = {{13,12,14},
                {27,26,25}};

void setup() {
  Serial.begin(115200);
  for (int i=0;i<2;i++){
      for (int n=0;n<3;n++){
        pinMode(rgb[i][n], OUTPUT);
        digitalWrite(rgb[i][n], HIGH);
      }
  }
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("Connected Firebase");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Serial.println("Setup Success");
}

void loop() {
  if (Firebase.ready() && signupOK) {
    Serial.println();
    Serial.println("Firebase Ready");
    int count = 7;
    String test;
    String path;
    String pathset;
    while (count <= 8){
      if(count<=9){
        path = "Parking/a0" + String(count) + "/state";
        pathset = "Parking/a0" + String(count) + "/timest";
      }
      else{
        path = "Parking/a" + String(count) + "/state";
        pathset = "Parking/a" + String(count) + "/timest";
      }
      //-----Read-----
      if (Firebase.RTDB.getInt(&fbdo, path)) {
        if (fbdo.dataType() == "int") {
          intStatus = fbdo.intData();
          Firebase.RTDB.getInt(&fbdo, pathset);
          intSet = fbdo.intData();
          Serial.println("PATH: " + fbdo.dataPath());
          Serial.println("Status: " + String(intStatus));
          Serial.println("Timest: " + String(intSet));
          if(intSet != 0){
            digitalWrite(rgb[count-7][0], HIGH);
            digitalWrite(rgb[count-7][1], HIGH);
            digitalWrite(rgb[count-7][2], LOW);
            Serial.println("1");
          }
          else if(intStatus == 255){
            digitalWrite(rgb[count-7][0], LOW);
            digitalWrite(rgb[count-7][1], HIGH);
            digitalWrite(rgb[count-7][2], HIGH);
            Serial.println("2");
          }
          else {
            digitalWrite(rgb[count-7][0], HIGH);
            digitalWrite(rgb[count-7][1], LOW);
            digitalWrite(rgb[count-7][2], HIGH);
            Serial.println("3");
          }
        }
      }
      else {
        Serial.println(fbdo.errorReason());
      }
      count++;
      delay(80);
    }
  }
  else {
    Serial.println("Firebase Not Ready");
  }
  delay(2000);
}
