#include <ESP8266WiFi.h> 


const char* ssid="test";   //Put your wifi network name here

const char* password = "1234567890";   //Put your wifi password here

void setup() {
  pinMode(D7,OUTPUT);
  Serial.begin(115200);      //initial Serial communication for serial monitor  Note:115200 depends on your board  

Serial.println(); 

 Serial.print("Wifi connecting to "); 

 Serial.println( ssid );
  WiFi.begin(ssid,password);  
  Serial.println();  Serial.print("Connecting");
  while( WiFi.status() != WL_CONNECTED )  //while loop runs repeatedly unless condition is false  
  {         
    digitalWrite(D7,HIGH);//it'll keep trying unless wifi is connected      
    delay(500);      
    Serial.print(".");          
    }
  Serial.println();
  Serial.println("Wifi Connected Success!");  

Serial.print("NodeMCU IP Address : ");   //Shows the IP (Internet Protocol) number of your NodeMcu 

 Serial.println(WiFi.localIP() );         //Gets the IP address of your Board  
 }
void loop()              //In our case we don't need this but still it needs to be there.
{  // put your main code here, to run repeatedly:
}
