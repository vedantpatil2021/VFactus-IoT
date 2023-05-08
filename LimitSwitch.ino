#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <FirebaseArduino.h>

int BUTTON_PIN = D5;
#define FIREBASE_HOST "iotproject-4d97d-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "xOhOD7cgR8PZvFZnbmX5kmPCR9le81gu4GmxeYj0"

const char *ssid =  "test";  
const char *pass =  "1234567890";


int buttonState = 0;

WiFiClient client;

void setup() {
  // put your setup code here, to run once:
  pinMode(BUTTON_PIN,OUTPUT);

  Serial.begin(9600);
  Serial.println("Connecting to ");
  Serial.println(ssid); 
 
  WiFi.begin(ssid, pass); 
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected"); 
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  
  if(Firebase.failed())
  {
    Serial.print(Firebase.error());
  }
}

void loop() {
buttonState = digitalRead(BUTTON_PIN);

  if (Firebase.getString("D2").toInt() == 1){
    if (buttonState == 1) {
      Firebase.setString("D0", "0");
      Firebase.setString("D1", "1");
    }
    else {
      Firebase.setString("D0", "1");
      Firebase.setString("D1", "0");
    }
  }
  else{
    Firebase.setString("D0", "0");
    Firebase.setString("D1", "0");    
  }
}
