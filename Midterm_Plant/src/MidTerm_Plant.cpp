/* 
 * Project Auto plant watering
 * Author: David Barbour
 * Date: 3/18/24
 */

#include "Particle.h"
#include "Adafruit_BME280.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"
#include "credentials.h"
#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include "Adafruit_MQTT/Adafruit_MQTT.h"
#include "Air_Quality_Sensor.h"

//system setup
SYSTEM_MODE(AUTOMATIC);
SYSTEM_THREAD(ENABLED);

//constants
const int PINPUMP = D16;
const int MOISTPIN = A2;
const int BUTPIN=D2;
const int OLED_RESET=-1;
const int DUSTPIN= D3;
const int AIRPIN = A0;
const int SAMPLETIME = 30000;


//display
Adafruit_SSD1306 display(OLED_RESET);

//for moisture sensor
int moistRead = 0;

//to determine if button is pushed
bool butPushed = false;
bool butUp=true;

//time syncing
String DateTime , TimeOnly ;

//bme 280 code (temp, pressure, humidity)
Adafruit_BME280 bme;
bool status;
float tempF;
float pressPA;
float humidRH;

//adafruit for publishing/subscribing
TCPClient TheClient; 
Adafruit_MQTT_SPARK mqtt(&TheClient,AIO_SERVER,AIO_SERVERPORT,AIO_USERNAME,AIO_KEY); 
Adafruit_MQTT_Subscribe subFeed = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/turnonpump"); 
Adafruit_MQTT_Publish pubFeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/plantdata");
int pumpOnOff;

//dust sensor
unsigned int duration,startTime,lowpulseoccupancy;
float ratio,concentration;
bool keepRunning;

//air quality sensor
AirQualitySensor sensor(AIRPIN);
int airQual;

//functions
void testPump();
void testDisplay();
void testBME();
void testSubFeed();
void testDust();
void testAir();

void MQTT_connect();
bool MQTT_ping();

void setup() {

  //start the serial monitor
  Serial.begin(9600);
  waitFor (Serial.isConnected,10000);

  //set pins for various sensors
  pinMode(PINPUMP,OUTPUT);  //pump
  pinMode(MOISTPIN,INPUT);  //moisture reader
  pinMode(BUTPIN,INPUT);    //button

  //start the display and time sync
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Time.zone (-6);
  Particle.syncTime();

  //start the BME 280
  status=bme.begin (0x76);
  if (status==false ) {Serial.printf ("BME280 at address %c failed to start ", 0x76 );}

  //start the subscription
  mqtt.subscribe(&subFeed);

  //dust sensor
  pinMode(DUSTPIN,INPUT);
}

void loop() {
  DateTime = Time.timeStr ();
  TimeOnly = DateTime.substring (11,19) ;

  // connect to the mqtt server
  MQTT_connect();
  MQTT_ping();
  //testPump();
  //testDisplay();
  //testBME();
  //testSubFeed();
  //testDust();
  testAir();
}

void testAir(){
  butPushed = digitalRead(BUTPIN);
  if (butPushed==true && butUp== true){
    airQual = sensor.getValue();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(10,0);
    display.clearDisplay();
    display.printf("Air Quality = %i \n",airQual);
    display.display();
    Serial.printf("Air Quality = %i \n",airQual);
  } 
  butUp=false;
  if (butPushed==false){butUp=true;}
}


void testDust(){
  //you're going to let this tie up the processor for 30 seconds 
  //to get a reading, so only do it every now and then
  keepRunning=true;
  lowpulseoccupancy=0;
  startTime = millis();
  
  //beginning display
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10,0);
  display.clearDisplay();
  
  display.printf("Starting reading");
  display.display();
  Serial.printf("Starting reading");

  while (keepRunning == true)
  {
    //bail out if you've been doing this for the sample time
    if(millis() > startTime + SAMPLETIME){keepRunning=false;}

    //count up the low pulses
    duration = pulseIn (DUSTPIN,LOW);
    lowpulseoccupancy = lowpulseoccupancy + duration ;
  }
  ratio = float(lowpulseoccupancy) /(float(SAMPLETIME) * 10.0) ;
  concentration = 1.1 * pow (ratio ,3) - 3.8 * pow (ratio,2) + 520 * ratio + 0.62;

  //display the concentration
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10,0);
  display.clearDisplay();
  
  display.printf("Dust concentration %.2f",concentration);
  display.display();
  Serial.printf("Dust concentration %.2f\n",concentration);

}


void testSubFeed(){
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(100))) 
  {
    if (subscription == &subFeed) 
    {
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(10,0);
      display.clearDisplay();
      pumpOnOff = atoi((char *)subFeed.lastread);
      if(pumpOnOff == 1)
      {
        digitalWrite(PINPUMP,HIGH);
        Serial.printf("Write High");
        display.printf("Pump on",tempF,248,pressPA,humidRH);
        display.display();
      }
      else
      {
        digitalWrite(PINPUMP,LOW);
        Serial.printf("Write Low");

        display.printf("Pump on",tempF,248,pressPA,humidRH);
        display.display();
      }
    }
  }
}

void testBME(){
  butPushed = digitalRead(BUTPIN);
  if (butPushed==true && butUp== true){
    tempF = (bme.readTemperature ()*9/5)+32.0; // deg F
    pressPA = (bme.readPressure () * 0.00029530); // pascals to inches of mercury
    humidRH = bme.readHumidity ();

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(10,0);
    display.clearDisplay();
    display.printf("Temp %0.1f%cF\n\n  Pressure %0.1f\n\n  Humid %0.1f \n",tempF,248,pressPA,humidRH);
    display.display();
  } 
  butUp=false;
  if (butPushed==false){butUp=true;}

}



void testDisplay(){
  butPushed = digitalRead(BUTPIN);
  if (butPushed==true && butUp== true){

      //read the moisture pin
      moistRead = analogRead(MOISTPIN);
      Serial.printf("Moisture %i\r",moistRead);

      //print name
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.printf("Moisture %i",moistRead);
      display.setCursor(0,20);
      display.printf("Time %s",TimeOnly.c_str());

      display.display();
  } 
  butUp=false;
  if (butPushed==false){butUp=true;}
}

void testPump(){
  digitalWrite(PINPUMP,HIGH);
  Serial.printf("Write High");
  delay(5000);
  digitalWrite(PINPUMP,LOW);
  Serial.printf("Write Low");
  delay(5000);
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;
 
  // Return if already connected.
  if (mqtt.connected()) {
    return;
  }
 
  Serial.print("Connecting to MQTT... ");
   while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.printf("Error Code %s\n",mqtt.connectErrorString(ret));
       Serial.printf("Retrying MQTT connection in 5 seconds...\n");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds and try again
  }
  Serial.printf("MQTT Connected!\n");
}

bool MQTT_ping() {
  static unsigned int last;
  bool pingStatus;

  if ((millis()-last)>120000) {
      //Serial.printf("Pinging MQTT \n");
      pingStatus = mqtt.ping();
      if(!pingStatus) {
        Serial.printf("Disconnecting \n");
        mqtt.disconnect();
      }
      last = millis();
  }
  return pingStatus;
}

