#include <TinyGPS++.h>
#include <SoftwareSerial.h>
//#include "WiFiClient.h"

#include <ESP8266WiFi.h>

//#include <FirebaseArduino.h>

#define jio_power 15
#define gps_power 14
#define TXPin 13
#define RXPin 12
#define rpi_power 5
#define mpu 4
#define timeout 6000

//static const int RXPin = 12, TXPin = 13;
static const uint32_t GPSBaud = 9600;
#define WIFI_SSID "JioFi3'S"
#define WIFI_PASSWORD "Satya@467"
int year , month , date, hour , minute , second;
String date_str , time_str;
int pm;

unsigned long start_time;
unsigned long last_time;
bool sleep;

/*
#define FIREBASE_HOST "driver-gps-aa25e.firebaseio.com"
#define FIREBASE_AUTH "ujW92oubKKuwTkkcoE60QKb1qtS0BEyB0HsmtBq2"
*/
// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void setup()
{
  Serial.begin(9600);
  ss.begin(GPSBaud);
  while(!Serial){}
  while(!ss){}
  
  //Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println();

  //Switch on gps
  pinMode(gps_power, OUTPUT);
  digitalWrite(gps_power, HIGH);
  Serial.println("Swithced on GPS");
  
  //Switch on Jio
  pinMode(jio_power, OUTPUT);
  digitalWrite(jio_power, HIGH);
  Serial.println("Switched on Jio");

  pinMode(rpi_power, INPUT);
  pinMode(mpu, INPUT);
  Serial.println("Wait for 10 seconds");
  delay(10000);
  /*
  // connect to wifi.
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
 
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
   if (Firebase.failed()) {  
      Serial.print("setting / message failed:");  
      Serial.println(Firebase.error());    
      return;  
  }  
  */
  start_time = millis();
  last_time = millis();
  Serial.println("Timeout started");
}

void loop()
{  
  while(millis() - last_time < timeout)
  {
    // This sketch displays information every time a new sentence is correctly encoded.
    while (ss.available() > 0)
    {
      if (gps.encode(ss.read()))
      {
        displayInfo();
        delay(1000);
        last_time = millis();
        Serial.println("Timeout updated");
      }
    }

    if(millis() - start_time > 30000)
    {
      if(digitalRead(rpi_power))
      {
        sleep = false;
        Serial.println("Wake mode");
      }
      else
      {
        sleep = true;
        Serial.println("Sleep mode");
      }    

      if(sleep)
      {   
        digitalWrite(jio_power, LOW);
        digitalWrite(gps_power, LOW);
        // Deep sleep mode for 30 seconds, the ESP8266 wakes up by itself when GPIO 16 (D0 in NodeMCU board) is connected to the RESET pin
        Serial.println("I'm awake, but I'm going into deep sleep mode for 30 seconds");
        ESP.deepSleep(30e6);
      }
      start_time = millis();
    }
    
  }
  if (millis() - last_time > timeout || gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected"));
    last_time = millis();
    Serial.println("Timeout updated");
  }
  
}

void displayInfo()
{
  Serial.print(F("\tLocation: ")); 
  if (gps.location.isValid())
  {

    double latitude = (gps.location.lat());
    double longitude = (gps.location.lng());
    
    String latbuf;
    latbuf += (String(latitude, 6));
    Serial.print(latbuf);
    Serial.print("   &   ");

    String lonbuf;
    lonbuf += (String(longitude, 6));
    Serial.println(lonbuf);
    
    /*
    Firebase.pushString("Driver2/Location", latbuf);                                //setup path and send readings
    Firebase.pushString("Driver2/Location", lonbuf);                                //setup path and send readings
    Firebase.setString("Driver2/Time_stamp_date",date_str);
    delay(1000);
    Firebase.setString("Driver2/Time_stamp",time_str);
    */
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  
  Serial.print(F("\tDate/Time: "));
  if (gps.date.isValid()) //check whether gps date is valid
  {
    date_str = "";
    date = gps.date.day();
    month = gps.date.month();
    year = gps.date.year();
    if (date < 10)
      date_str = '0';
    date_str += String(date);// values of date,month and year are stored in a string
    date_str += " / ";

    if (month < 10)
      date_str += '0';
    date_str += String(month); // values of date,month and year are stored in a string
    date_str += " / ";
    if (year < 10)
      date_str += '0';
    date_str += String(year); // values of date,month and year are stored in a string
    Serial.print(date_str );
    Serial.print("   &   ");
  }
  
  if (gps.time.isValid())  //check whether gps time is valid
  {
    time_str = "";
    hour = gps.time.hour();
    minute = gps.time.minute();
    second = gps.time.second();
    minute = (minute + 30); // converting to IST
    if (minute > 59)
    {
      minute = minute - 60;
      hour = hour + 1;
    }
    hour = (hour + 5) ;
    if (hour > 23)
      hour = hour - 24;   // converting to IST
    if (hour >= 12)  // checking whether AM or PM
      pm = 1;
    else
      pm = 0;
    hour = hour % 12;
    if (hour < 10)
      time_str = '0';
    time_str += String(hour); //values of hour,minute and time are stored in a string
    time_str += " : ";
    if (minute < 10)
      time_str += '0';
    time_str += String(minute); //values of hour,minute and time are stored in a string
    time_str += " : ";
    if (second < 10)
      time_str += '0';
    time_str += String(second); //values of hour,minute and time are stored in a string
    if (pm == 1)
      time_str += " PM ";
    else
      time_str += " AM ";
    Serial.print(time_str);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}
