#include <SoftwareSerial.h> // Including the software serial library for ESP module, referred from https://docs.arduino.cc/learn/built-in-libraries/software-serial
#include "DHT.h"; // Including DHT.h library for sensing temparature and humidity, referred from https://learn.adafruit.com/dht/using-a-dhtxx-sensor
#define typeOfDhtSensor DHT11 // defining the typeOfDhtSensor
#define dhtDigitalPin 7 // defining the pin number of dhtDigitalPin
DHT dhtTempAndHumidity(dhtDigitalPin, typeOfDhtSensor); // sending the required data to dht_temp_and_humidity class
#define receiveDataPin 3 // defining pin number of receiveDataPin for ESP 8266 Wi-Fi Module

#define transferDataPin 2 // defining pin number of transferDataPin for ESP 8266 Wi-Fi Module
//below constant variables are used for establishing connection between ESP module and Arduino microcontroller
const String username = "XXXXXXXXXXXXXX"; // storing the usernmae or SSID
const String password = "XXXXXXXXXXXX"; // storing the password into a varibale named password
const String apiKey = "TBOC7YZWU6WK2M5S"; // assignment of the API key to transmit the data to thingSpeak channel
const String domainHost = "api.thingspeak.com"; // storing domain name of the thingSpeak to fetch the thingSpeak server
const String portNumber = "80"; // port number 80 is by default accepted by the thingSpeak server to establish a successful connection.

int successCountOne; int successCountPeriod; boolean detected = false;

SoftwareSerial esp8266WiFiModule(receiveDataPin,transferDataPin);

void setup() {
  dhtTempAndHumidity.begin(); // using the begin function from dht library and starting the sensor.
  Serial.begin(9600); esp8266WiFiModule.begin(115200); // using the begin function from dht library, softwareserial and starting the sensor. giving the baud rate as well. 
  // Learned how to connect to internet for ESP 8266 using AT commands, referred from https://electronics-fun.com/esp8266-at-commands/
  forwardDataUsingAT("AT",1,"OK"); forwardDataUsingAT("AT+CWMODE=1",1,"OK"); forwardDataUsingAT(String("AT+CWJAP='") + username + "','" + password + "'", 10, "OK");
  delay(500); // Allowing a timeframe of 500 milliseconds to connect the module to the network
}

void loop() {
  float humidityValue = dhtTempAndHumidity.readHumidity(); // using readHumidity function from DHT library
  float temparatureValue = dhtTempAndHumidity.readTemperature(); // using readTemperature function from DHT library
  float temparatureValueInF = (temparatureValue * 1.8) + 32; // converting temperature from celsuis to farenheit
  int airQualityValue = analogRead(A0); // Reading the input from the analog pin 0
  float gasValue = analogRead(A1); // Reading the input from the analog pin 1
  String getEnvironmentalData = "GET https://domainHost/update?api_key="+ apiKey +"&field1="+temparatureValueInF+"&field2="+humidityValue+"&field3="+airQualityValue+"&field4="+gasValue;
  Serial.print("Temperature =" + String(temparatureValueInF) + "°F ");
  Serial.print("Humidity =" + String(humidityValue) + "% ");
  Serial.print("Gas = " + String(gasValue) + " PPM ");
  // Learned how to connect to internet for ESP 8266 using AT commands, referred from https://electronics-fun.com/esp8266-at-commands/
  forwardDataUsingAT("AT+CIPMUX=1",5,"OK"); forwardDataUsingAT("AT+CIPSTART=0,\"TCP\",\""+ domainHost +"\","+ portNumber,15,"OK"); forwardDataUsingAT("AT+CIPSEND=0," +String(getEnvironmentalData.length()+4),4,">");
  esp8266WiFiModule.println(getEnvironmentalData);
  delay(1500); // Allowing a timeframe of 1500 milliseconds for delay, we send the data to thingspeak every 15 seconds.
  successCountOne = successCountOne + 1;
  forwardDataUsingAT("AT+CIPCLOSE=0",5,"OK");
  delay(500); // Allowing a timeframe of 500 milliseconds for delay
}

void forwardDataUsingAT(String passedCommand, int maximumTime, char scanReplay[]) {
  Serial.print(successCountOne + ": The Given command -->  " + passedCommand + " ");
  while(successCountPeriod < (maximumTime*1)){ esp8266WiFiModule.println(passedCommand); if(esp8266WiFiModule.find(scanReplay)){ detected = true; break; } successCountPeriod++; }
  if(detected){ Serial.println("Connected Successfully"); successCountOne++; successCountPeriod = 0; }
  else{ Serial.println("It got Failed"); successCountOne = 0; successCountPeriod = 0; } detected = false;
 }
