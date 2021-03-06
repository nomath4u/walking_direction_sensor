#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "secret.h"


#define PIR1_PIN D3 //Closer to door
#define PIR2_PIN D8 //Further from door

void MQTT_connect(); //ESP bug 
void send_direction(int);

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish direct_sensor = Adafruit_MQTT_Publish(&mqtt, DIRECTION_FEED);

enum{ IN, OUT };

void setup() { 
  Serial.begin(9600);
//Statup wifi connection
  WiFi.mode(WIFI_STA); //Don't let it act as an access point
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println(".");
    delay(200);
  }
}

void loop() {
  int pin1;
  int pin2;
  static int previous_pin1 = 0;
  static int previous_pin2 = 0;
  static bool cooldown = 0; //Shouldn't technically be needed but seems to help
  pinMode(LED_BUILTIN, OUTPUT);
  MQTT_connect();
  
  pin1 = digitalRead(PIR1_PIN);
  pin2 = digitalRead(PIR2_PIN);

  if(pin1){
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    digitalWrite(LED_BUILTIN, HIGH);

  }
  
  if(pin1 != previous_pin1){

   if( pin1 && pin2 && !cooldown){ //Just hit first sensor but secon was already on
      Serial.println("Sending IN");
      send_direction(IN);
      cooldown = 1;
    }
  }

  if(pin2 != previous_pin2){

   if( pin2 && pin1 && !cooldown){ //Just Hit second sensor and first was already on
      Serial.println("Sending OUT");
      send_direction(OUT);
      cooldown = 1;
   }    
  } 

  if( !pin1 && !pin2 ){
    cooldown = 0; //Both sensors off we are ready to trigger again
  }
  previous_pin1 = pin1;
  previous_pin2 = pin2;

  //Serial.print("PINS: ");
  //Serial.print(pin1); Serial.println(pin2);
  delay(50);
}

void send_direction(int d){
  direct_sensor.publish(d);
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
