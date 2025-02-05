#include <Arduino.h> 
#include <WiFi.h> 
#include <HTTPClient.h> 
// WiFi credentials 
const char* ssid = "Keerthi";           
const char* password = "12345678";   
// ThingSpeak API  
const char* server = "http://api.thingspeak.com/update"; 
const char* apiKey = "7K9BYT6SH4B4F3HJ";  //ThingSpeak API Key 
// pH Sensor Configuration 
const int phSensorPin = 26;   // Analog pin for pH sensor ('To') 
float minPH = 7.10;          
float maxPH = 7.15;          
float currentPH = minPH;      
bool increasing = true;      
 // Minimum pH value 
 // Maximum pH value 
// Start pH value at the minimum 
 // Direction of change 



// Flow Sensor Configuration 
const int flowSensorPin = 15; 
volatile int pulseCount = 0; 
float flowRate = 0; 
// TDS Sensor Configuration (Simulated range: 70 - 80) 
const int tdsSensorPin = 34; 
int minTDS = 70;              
int maxTDS = 80;              
// Minimum TDS value 
// Maximum TDS value 
// Temperature Configuration  
float temperatureC = 0.0;     // Variable for temperature 
// Variables for flow calculation 
unsigned long previousMillis = 0; 
const unsigned long interval = 3000; 
// Update interval in milliseconds (3 seconds) 
// Interrupt function for flow sensor 
void IRAM_ATTR countPulses() { 
pulseCount++; 
} 



void setup() { 
Serial.begin(115200); 
// Connect to WiFi 
Serial.print("Connecting to WiFi"); 
WiFi.begin(ssid, password); 
while (WiFi.status() != WL_CONNECTED) { 
delay(500); 
Serial.print("."); 
} 
Serial.println("\nConnected to WiFi"); 
// Flow Sensor Setup 
pinMode(flowSensorPin, INPUT); 
attachInterrupt(digitalPinToInterrupt(flowSensorPin), countPulses, RISING); 
randomSeed(analogRead(0));  // Initialize random seed for TDS simulation 
} 
float readPH() { 
// Simulate pH value fluctuating between 7.10 and 7.15 
if (increasing) { 
currentPH += 0.001; 
if (currentPH >= maxPH) { 


 
increasing = false;  // Switch direction to decrease 
} 
} else { 
currentPH -= 0.001; 
if (currentPH <= minPH) { 
increasing = true;  // Switch direction to increase 
} 
} 
return currentPH; 
} 
float calculateFlowRate() { 
float calibrationFactor = 4.5;  // Adjust based on your sensor’s specification 
float rate = (pulseCount / calibrationFactor);  // Flow rate in liters per minute 
pulseCount = 0;  // Reset pulse count for the next interval 
return rate; 
} 
int simulateTDS() { 
return random(minTDS, maxTDS + 1);  
} 



float simulateTemperature() { 
return random(14, 19);  
} 
void sendDataToThingSpeak(float ph, float flow, int tds, float temp) { 
if (WiFi.status() == WL_CONNECTED) { 
HTTPClient http; 
String url = String(server) + "?api_key=" + apiKey + 
"&field1=" + String(ph) + 
"&field2=" + String(flow) + 
"&field3=" + String(tds) + 
"&field4=" + String(temp); 
http.begin(url); 
int httpResponseCode = http.GET(); 
if (httpResponseCode > 0) { 
Serial.print("Data sent to ThingSpeak, response code: "); 
Serial.println(httpResponseCode); 
} else { 
Serial.print("Error sending data to ThingSpeak, error code: "); 
Serial.println(httpResponseCode); 
} 
http.end(); 



} else { 
Serial.println("WiFi not connected"); 
} 
} 
void loop() { 
unsigned long currentMillis = millis(); 
if (currentMillis - previousMillis >= interval) { 
previousMillis = currentMillis; 
// Read simulated pH value 
float ph = readPH(); 
// Calculate flow rate from sensor pulses 
flowRate = calculateFlowRate(); 
// Simulate TDS value within 70 to 80 
int tds = simulateTDS(); 
// Simulate temperature value between 14 and 18 
temperatureC = simulateTemperature(); 



// Print pH, flow rate, TDS, and temperature values to the serial monitor 
Serial.print("pH: "); 
Serial.print(ph); 
Serial.print(" | Flow rate: "); 
Serial.print(flowRate); 
Serial.print(" L/min | TDS: "); 
Serial.print(tds); 
Serial.print(" ppm | Temperature: "); 
Serial.print(temperatureC); 
Serial.println(" °C"); 
// Send data to ThingSpeak 
sendDataToThingSpeak(ph, flowRate, tds, temperatureC); 
} 
}
