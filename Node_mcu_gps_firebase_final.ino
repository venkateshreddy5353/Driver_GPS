#include <TinyGPS++.h>
#include <SoftwareSerial.h>
//#include "WiFiClient.h"

#include <ESP8266WiFi.h>

#include <FirebaseArduino.h>

static const int RXPin = 12, TXPin = 13;
static const uint32_t GPSBaud = 9600;
#define WIFI_SSID "JioFi3'S"
#define WIFI_PASSWORD "Satya@467"
int year , month , date, hour , minute , second;
String date_str , time_str;
int pm;

#define FIREBASE_HOST "driver-gps-aa25e.firebaseio.com"
#define FIREBASE_AUTH "ujW92oubKKuwTkkcoE60QKb1qtS0BEyB0HsmtBq2"
// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void setup()
{
  Serial.begin(115200);
  ss.begin(GPSBaud);
  Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println();
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
      Serial.print("setting /message failed:");  
      Serial.println(Firebase.error());    
      return;  
  }  
  
}

void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
}

void displayInfo()
{
 // Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {

    double latitude = (gps.location.lat());
    double longitude = (gps.location.lng());
    
    String latbuf;
    latbuf += (String(latitude, 6));
    Serial.println(latbuf);

    String lonbuf;
    lonbuf += (String(longitude, 6));
    Serial.println(lonbuf);
    delay(2000);
    
  Firebase.pushString("Driver2/Location", latbuf);                                  //setup path and send readings
  Firebase.pushString("Driver2/Location", lonbuf);                                //setup path and send readings
  Firebase.setString("Driver2/Time_stamp_date",date_str);
  delay(1000);
  Firebase.setString("Driver2/Time_stamp",time_str);
    
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
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
