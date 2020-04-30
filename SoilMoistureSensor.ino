#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

//Store wifi credentials.
const String ssid = "*";
const String password = "*";

//Set what min value = 0% moisture and max = 100% moisture
const int AirValue = 849;   //you need to replace this value with Value_1
const int WaterValue = 455;  //you need to replace this value with Value_2
int soilMoistureValue = 0;
int soilmoisturepercent = 0;

//Set sensor identification information
const String locationID="1";
const String sensorID="1";

//Set the access pin for the moisture sensor.
const int SensorPin = A0;

//Set API endpoint
const String apiUrl = "http://192.168.1.205:5000/api/Sensor";

HTTPClient http;

//One time setup required for initializing the ESP
void setup() {
  Serial.begin(115200); // open serial port, set the baud rate to 115200 bps

  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.println("Connecting...");
  }

  //Only bother doing any work if the wifi is connected.
  if (WiFi.status() == WL_CONNECTED)
  {
    //Section to get soil sensor reading.
    soilMoistureValue = analogRead(SensorPin);  //put Sensor insert into soil
    Serial.println("Sensor value:" + (String)soilMoistureValue);
    soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
    if(soilmoisturepercent > 100)
    {
      Serial.println("100 %");
    }
    else if(soilmoisturepercent <0)
    {
      Serial.println("0 %");
    }
    else if(soilmoisturepercent >0 && soilmoisturepercent < 100)
    {
      Serial.print(soilmoisturepercent);
      Serial.println("%");
    }

    //Section to Send sensor reading
    http.begin(apiUrl);
    http.setTimeout(10000);
    http.addHeader("Content-Type", "text/json");
    int httpCode = http.POST("{\"LocationID\":" + locationID + ",\"SensorID\":" + sensorID + ",\"Moisture\":" + soilmoisturepercent + "}");
    //int httpCode = http.GET();
    String payload = http.getString();
    //Serial.println("Posted:{\"LocationID\":" + locationID + ",\"SensorID\":" + sensorID + ",\"Moisture\":" + soilmoisturepercent + "}");
    Serial.println("Response Code:" + (String)httpCode);
    Serial.println("Received:" + payload);
    http.end();
  }
  else
  {
    Serial.print("Wifi not Connected");
  }

  //wait time till we attempt to get reading again.
  //delay(120000);
  Serial.println();

  // Deep sleep mode for 30 seconds, the ESP8266 wakes up by itself when GPIO 16 (D0 in NodeMCU board) is connected to the RESET pin
  Serial.println("Completed, but I'm going into deep sleep mode for 30 seconds");
  ESP.deepSleep(30e6); 
}

void loop() {
  
}
