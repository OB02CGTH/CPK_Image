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
int rgb[3][3] = {{13,12,14}, 
                {27,26,25}, 
                {15,2,4}};
//int rgb[6][3] ={{13,12,14},{27,26,25},{15,2,0}};
//#define r1 13
//#define g1 12
//#define b1 14
//#define r2 26
//#define g2 25
//#define b2 33
//#define r3 35
//#define g3 34
//#define b3 39
//#define r4 15
//#define g4 2
//#define b4 4
//#define r5 17
//#define g5 5
//#define b5 18
//#define r6 21
//#define g6 3
//#define b6 1

void setup() {
  Serial.begin(115200);
  for (int i=0;i<3;i++){
      for (int n=0;n<3;n++){
        pinMode(rgb[i][n], OUTPUT);
        digitalWrite(rgb[i][n], HIGH);
      }
////    pinMode(r1, OUTPUT);pinMode(g1, OUTPUT);pinMode(b1, OUTPUT);
////    pinMode(r2, OUTPUT);pinMode(g2, OUTPUT);pinMode(b2, OUTPUT);
////    pinMode(r3, OUTPUT);pinMode(g3, OUTPUT);pinMode(b3, OUTPUT);
////    pinMode(r4, OUTPUT);pinMode(g4, OUTPUT);pinMode(b4, OUTPUT);
////    pinMode(r5, OUTPUT);pinMode(g5, OUTPUT);pinMode(b5, OUTPUT);
////    pinMode(r6, OUTPUT);pinMode(g6, OUTPUT);pinMode(b6, OUTPUT);
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
    int count = 1;
    String test;
    String path;
    String pathset;
    while (count <= 3){
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
          if (intSet != 0){
            digitalWrite(rgb[count-1][0], HIGH);
            digitalWrite(rgb[count-1][1], HIGH);
            digitalWrite(rgb[count-1][2], LOW);
          }
          else if(intStatus == 255){
            digitalWrite(rgb[count-1][0], LOW);
            digitalWrite(rgb[count-1][1], HIGH);
            digitalWrite(rgb[count-1][2], HIGH);
          }
          else {
            digitalWrite(rgb[count-1][0], HIGH);
            digitalWrite(rgb[count-1][1], LOW);
            digitalWrite(rgb[count-1][2], HIGH);
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
