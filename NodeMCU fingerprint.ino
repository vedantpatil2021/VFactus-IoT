/***************************************************
  This is an example sketch for our optical Fingerprint sensor

  Designed specifically to work with the Adafruit BMP085 Breakout
  ----> http://www.adafruit.com/products/751

  These displays use TTL Serial to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/


#include <Adafruit_Fingerprint.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <FirebaseArduino.h>


#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
#define Finger_Rx 14 //D5
#define Finger_Tx 12 //D6

SoftwareSerial mySerial(Finger_Rx,Finger_Tx);

LiquidCrystal_I2C lcd(0x27, 16, 2);

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif

const char *ssid =  "test";  
const char *pass =  "1234567890";

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

WiFiClient client;

void setup()
{
  Serial.begin(9600);
  pinMode(D4,OUTPUT);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");
  Wire.begin(D2,D1);
	lcd.begin();
	lcd.backlight();
  lcd.print("Welcome"); 
  delay(1000);
  lcd.clear();
  
  // Wifi
  WiFi.begin(ssid, pass); 
  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

  finger.getTemplateCount();

  if (finger.templateCount == 0) {
    Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' example.");
  }
  else {
    Serial.println("Waiting for valid finger...");
      Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  }

  // Firebase
  Firebase.begin("iotproject-4d97d-default-rtdb.firebaseio.com", "xOhOD7cgR8PZvFZnbmX5kmPCR9le81gu4GmxeYj0");
  if(Firebase.failed())
  {
    Serial.print(Firebase.error());
  }    


  // Firebase
  Firebase.setString("D0", "0");
  Firebase.setString("D1", "0");
  Firebase.setString("D2", "0");
}

void loop()                     // run over and over again
{
  getFingerprintID();
  delay(50);            //don't ned to run this at full speed.
  digitalWrite(D4, Firebase.getString("LED13").toInt());
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      // lcd.print("Image taken"); 
      // delay(1000);
      // lcd.clear();
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      // lcd.print("No finger Detected");
      // delay(1000);
      // lcd.clear();
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.print("Communication");
      lcd.setCursor(0,1); 
      lcd.print("Error");
      lcd.clear();
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      lcd.print("Imaging error");
      lcd.clear();
      Serial.println("Imaging error");
      Serial.println("Imaging error");
      return p;
    default:
      lcd.setCursor(0,0);  
      lcd.print("Unknown error");  
      lcd.clear();  
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      // lcd.setCursor(0,0);  
      // lcd.print("Image Conv");
      // delay(1000);
      // lcd.clear();
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      lcd.setCursor(0,0);  
      lcd.print("Immage Messy");
      delay(1000); 
      lcd.clear();
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      lcd.setCursor(0,0);  
      lcd.print("Com Error");
      delay(1000); 
      lcd.clear();
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      lcd.setCursor(0,0);  
      lcd.print("CNFF");
      delay(3000); 
      lcd.clear();
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      lcd.setCursor(0,0);  
      lcd.print("CNFFF");
      delay(1000); 
      lcd.clear();
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      lcd.setCursor(0,0);  
      lcd.print("Unknown error");
      delay(1000); 
      lcd.clear();
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    // lcd.setCursor(0,0);  
    // lcd.print("Match Found");
    // delay(1000);
    // lcd.clear();
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  if(finger.fingerID == 5){
    lcd.print("Vedant Patil");
    delay(1000);
    lcd.clear();
    Firebase.setString("D2", "1");
  }  
  // lcd.print("Found:");
  // lcd.setCursor(0, 1);
  // lcd.print(finger.fingerID);
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  if(finger.fingerID == 5){
    lcd.print("Vedant Patil");
    delay(1000);
    lcd.clear();
    Firebase.setString("D2", "1");
  }
  // lcd.print(finger.fingerID);
  // delay(1000);
  // lcd.clear();
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}
