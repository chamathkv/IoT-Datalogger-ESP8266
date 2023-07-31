////////////////////////////
////////////////////////////
////////////////////////////
////////////////////////////
////////////////////////////
//Code used to get temperature and moisture readings from DS18B20 temperature sensors and YL-69 moisture sensors.//
////////////////////////////
////////////////////////////
////////////////////////////
////////////////////////////
////////////////////////////
//****************************Connections*****************************//
//D0 ----------> EN   (Mux)

//D1 ----------> SCL  (Screen)
//D2 ----------> SDA  (Screen)
//D3 ----------> Temp Data   (DS18B20)
//D4 ----------> S0   (Mux)
//D5 ----------> S1   (Mux)
//D6 ----------> S2   (Mux)

//D7 ----------> S3   (Mux)
//D8 ----------> Relay
//A0 ----------> Z    (Mux)
//********************************************************************//

//******************************DS18B20******************************//
#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 0
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress sensor1 =  {0x28, 0x90, 0x7F, 0xC7, 0x1F, 0x13, 0x1, 0xE0 }; //34
DeviceAddress sensor2 = { 0x28, 0xAA, 0x5E, 0x50, 0x19, 0x13, 0x2, 0xA3 }; //04
DeviceAddress sensor3 = { 0x28, 0x60, 0xF4, 0xB8, 0x1F, 0x13, 0x1, 0x1B }; //05
DeviceAddress sensor4 = { 0x28, 0xAA, 0xA2, 0xC, 0x19, 0x13, 0x2, 0xEE }; //03
DeviceAddress sensor5 = { 0x28, 0xAA, 0x9B, 0xF, 0x19, 0x13, 0x2, 0x11 }; //01
DeviceAddress sensor6 = { 0x28, 0xAA, 0x89, 0xF, 0x19, 0x13, 0x2, 0xEE }; //30
DeviceAddress sensor7 = { 0x28, 0xAA, 0xA8, 0xBB, 0x18, 0x13, 0x2, 0xEF }; //02
DeviceAddress sensor8 = { 0x28, 0xAA, 0xC5, 0x5D, 0x19, 0x13, 0x2, 0xF9 }; //32
DeviceAddress sensor9 = { 0x28, 0x36, 0x74, 0x45, 0x92, 0x18, 0x2, 0x13 }; //22
DeviceAddress sensor10 = { 0x28, 0x4F, 0xD2, 0x3C, 0x2F, 0x14, 0x1, 0x8A }; //31
DeviceAddress sensor11 = { 0x28, 0x2, 0x92, 0x45, 0x92, 0x18, 0x2, 0x64 }; //21
DeviceAddress sensor12 = { 0x28, 0xAA, 0x1D, 0x0, 0x19, 0x13, 0x2, 0xDC }; //33
DeviceAddress sensor13 = { 0x28, 0x7B, 0x1B, 0x45, 0x92, 0x5, 0x2, 0x37 }; //28
DeviceAddress sensor14 = { 0x28, 0xB6, 0x0, 0x45, 0x92, 0x11, 0x2, 0x39 }; //14
//********************************************************************//

//******************************74HC4067******************************//
#include "MUX74HC4067.h"
//********************************************************************//

//*******************************SCREEN*******************************//
//SCL ----------> D1
//SDA ----------> D2

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     LED_BUILTIN //4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//********************************************************************//

//******************************74HC4067******************************//
// Creates a MUX74HC4067 instance
// 1st argument is the Arduino PIN to which the EN pin connects
// 2nd-5th arguments are the Arduino PINs to which the S0-S3 pins connect
//MUX74HC4067 mux(7, 8, 9, 10, 11); //Arduino
MUX74HC4067 mux(16, 2, 14, 12, 13);    //NodeMCU
//********************************************************************//

//******************************Internet******************************//
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>

const char* ssid = "ENTER WIFI SSID";
const char* password = "ENTER WIFI PASSWORD";

WiFiClient client;
unsigned long Channel1 = 1372657;  //2021 Moisture Set 1
const char * myWriteAPIKey1 = "ENTER API KEY";
unsigned long Channel2 = 1343617; //2021 Moisture Set 2
const char * myWriteAPIKey2 = "ENTER API KEY";
unsigned long Channel3 = 1343618;  //2021 Moisture Set 3
const char * myWriteAPIKey3 = "ENTER API KEY";
unsigned long Channel4 = 1343600; //2021 Temperature Set 1
const char * myWriteAPIKey4 = "ENTER API KEY";
unsigned long Channel5 = 1343601; //2021 Temperature Set 2
const char * myWriteAPIKey5 = "ENTER API KEY";
//********************************************************************//

//*****************************Constants******************************//
int timeBetweenReadings  = 90000; //1 minute 30 seconds
//int delayForNewReadingCycle = 900000; //15 minutes
//int delayForNewReadingCycle = 1800000; //30 minutes
int delayForNewReadingCycle = 2400000; //40 minutes
int MuxValues[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//********************************************************************//

//**************************Important pins****************************//
#define relayPin 15
//********************************************************************//

void startup()
{
  Serial.println(".");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  // Print the IP address
  Serial.println(WiFi.localIP());
  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);
  delay(10000);
}
//********************************************************************//

void readMuxVal(int x) {
  int data = 0;
  int mPerc = 0;
  data = mux.read(x);   // Reads from channel i. Returns a value from 0 to 1023
  mPerc=(-0.000000001219500040736946*(pow(data,4)))+(0.000002433251912455929*(pow(data,3)))+(-0.001531281295811*(pow(data,2)))+(0.227285203001156*(data))+(102.9068673556674);
  Serial.print("Potentiometer at channel ");
  Serial.print(x);
  Serial.print(" is at ");
  Serial.print(data);
  Serial.print(" (");
  Serial.print((double)(data) * 100 / 1023);
  Serial.print("%%");
  Serial.println(")");

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  if (x >= 0 && x <= 4) display.setCursor(10, ((8 * x) + 17));
  if (x >= 5 && x <= 7) display.setCursor(70, ((8 * x) - 23));
  if (x >= 8 && x <= 11) display.setCursor(10, ((8 * x) - 47));
  if (x >= 12 && x <= 15) display.setCursor(70, ((8 * x) - 79));
  display.print("Y");
  display.print(x);
  display.print(":");
  display.print(mPerc);
  display.print("%");
  if (x >= 0 && x <= 3) 
  {
    ThingSpeak.writeField(Channel1, (x+1), mPerc, myWriteAPIKey1);
    delay (40000);
    ThingSpeak.writeField(Channel1, (x+5), data, myWriteAPIKey1);
  }
  if (x >= 4  && x <= 7) 
  {
    ThingSpeak.writeField(Channel2, (x-3), mPerc, myWriteAPIKey2);
    delay (40000);
    ThingSpeak.writeField(Channel2, (x+1), data, myWriteAPIKey2);
  }
  if (x >= 8 && x <= 11) 
  {
    ThingSpeak.writeField(Channel3, (x-7), mPerc, myWriteAPIKey3);
    delay (40000);
    ThingSpeak.writeField(Channel3, (x-3), data, myWriteAPIKey3);
  }
 
  display.display();
  MuxValues[x] = mPerc;
  delay (timeBetweenReadings);
}


void setup()
{
  //******************************74HC4067******************************//
  Serial.begin(9600);  // Initializes serial port
  // Waits for serial port to connect. Needed for Leonardo only
  while ( !Serial ) ;

  // Configures how the SIG pin will be interfaced
  // e.g. The SIG pin connects to PIN A0 on the Arduino,
  //      and PIN A0 is a analog input
  mux.signalPin(A0, INPUT, ANALOG);
  //********************************************************************//

  //*******************************SCREEN*******************************//
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println("SSD1306 allocation failed");
    for (;;); // Don't proceed, loop forever
  }
  //********************************************************************//

  //*******************************RELAY********************************//
  pinMode(relayPin, OUTPUT);
  //********************************************************************//

  //******************************DS18B20******************************//
  sensors.begin();
  //********************************************************************//


}

// Reads the 16 channels and reports on the serial monitor
// the corresponding value read by the A/D converter
void loop()
{
  digitalWrite(relayPin, LOW);
  
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,20);             // Start at top-left corner
  display.print("Connecting...");
  display.print(ssid);
  display.display();

  startup();

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi disconnected, reconnecting now.");
    startup();
  }

  float data = 0;

  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0, 0);            // Start at top-left corner
  display.println("Connected to");
  display.print(ssid);

  delay(60000);
  digitalWrite(relayPin, HIGH);
  readMuxVal(0);
  readMuxVal(1);
  readMuxVal(2);
  readMuxVal(3);
  readMuxVal(4);
  readMuxVal(5);
  readMuxVal(6);
  readMuxVal(7);

  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0, 0);            // Start at top-left corner
  display.println("Connected to");
  display.print(ssid);
  
  readMuxVal(8);
  readMuxVal(9);
  readMuxVal(10);
  readMuxVal(11);

  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0, 0);            // Start at top-left corner
  display.println("Connected to");
  display.print(ssid);

  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");

  Serial.print("Sensor 1(*C): ");
  data = sensors.getTempC(sensor1);
  Serial.println(data);
  if (data != 85 || data != -127)
    {
      ThingSpeak.writeField(Channel4, (1), data, myWriteAPIKey4);      
    }
  display.setCursor(10, 24);
  display.print("T1: ");// Start at top-left corner
  display.print(data);
  display.display();
  delay (timeBetweenReadings);

  Serial.print("Sensor 2(*C): ");
  data = sensors.getTempC(sensor2);
  Serial.println(data);
  if (data != 85 || data != -127)
    {
      ThingSpeak.writeField(Channel4, (2), data, myWriteAPIKey4);   
    }
  display.setCursor(10, 32);
  display.print("T2: ");// Start at top-left corner
  display.print(data);
  display.display();
  delay (timeBetweenReadings);

  digitalWrite(relayPin, LOW);  //RELAY PIN SWITCHES OFF HERE
  
  Serial.print("Sensor 3(*C): ");
  data = sensors.getTempC(sensor3);
  Serial.println(data);
  if (data != 85 || data != -127)
    {
      ThingSpeak.writeField(Channel4, (3), data, myWriteAPIKey4);   
    }
  display.setCursor(10, 40);
  display.print("T3: ");// Start at top-left corner
  display.print(data);
  display.display();
  delay (timeBetweenReadings);

  Serial.print("Sensor 4(*C): ");
  data = sensors.getTempC(sensor4);
  Serial.println(data);
  if (data != 85 || data != -127)
    {
      ThingSpeak.writeField(Channel4, (4), data, myWriteAPIKey4);   
    }
  display.setCursor(10, 48);
  display.print("T4: ");// Start at top-left corner
  display.print(data);
  display.display();
  delay (timeBetweenReadings);

  Serial.print("Sensor 5(*C): ");
  data = sensors.getTempC(sensor5);
  Serial.println(data);
  if (data != 85 || data != -127)
    {
      ThingSpeak.writeField(Channel4, (5), data, myWriteAPIKey4);   
    }
  display.setCursor(10, 56);
  display.print("T5: ");// Start at top-left corner
  display.print(data);
  display.display();
  delay (timeBetweenReadings);

  Serial.print("Sensor 6(*C): ");
  data = sensors.getTempC(sensor6);
  Serial.println(data);
  if (data != 85 || data != -127)
    {
      ThingSpeak.writeField(Channel4, (6), data, myWriteAPIKey4);   
    }
  display.setCursor(70, 24);
  display.print("T6: ");// Start at top-left corner
  display.print(data);
  display.display();
  delay (timeBetweenReadings);

  Serial.print("Sensor 7(*C): ");
  data = sensors.getTempC(sensor7);
  Serial.println(data);
  if (data != 85 || data != -127)
    {
      ThingSpeak.writeField(Channel4, (7), data, myWriteAPIKey4);   
    }
  display.setCursor(70, 32);
  display.print("T7: ");// Start at top-left corner
  display.print(data);
  display.display();
  delay (timeBetweenReadings);

  Serial.print("Sensor 8(*C): ");
  data = sensors.getTempC(sensor8);
  Serial.println(data);
  if (data != 85 || data != -127)
    {
      ThingSpeak.writeField(Channel4, (8), data, myWriteAPIKey4);   
    }
  display.setCursor(70, 40);
  display.print("T8: ");// Start at top-left corner
  display.print(data);
  display.display();
  delay (timeBetweenReadings);

  Serial.print("Sensor 9(*C): ");
  data = sensors.getTempC(sensor9);
  Serial.println(data);
  if (data != 85 || data != -127)
    {
      ThingSpeak.writeField(Channel5, (1), data, myWriteAPIKey5);   
    }
  display.setCursor(70, 48);
  display.print("T9: ");// Start at top-left corner
  display.print(data);
  display.display();
  delay (timeBetweenReadings);

  Serial.print("Sensor 10(*C): ");
  data = sensors.getTempC(sensor10);
  Serial.println(data);
  if (data != 85 || data != -127)
    {
      ThingSpeak.writeField(Channel5, (2), data, myWriteAPIKey5); 
    }
  display.setCursor(70, 56);
  display.print("T10: ");// Start at top-left corner
  display.print(data);
  display.display();
  delay (timeBetweenReadings);

  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0, 0);            // Start at top-left corner
  //display.print("Readings");
  display.println("Connected to");
  display.print(ssid);

  Serial.print("Sensor 11(*C): ");
  data = sensors.getTempC(sensor11);
  Serial.println(data);
  if (data != 85 || data != -127)
    {
      ThingSpeak.writeField(Channel5, (3), data, myWriteAPIKey5); 
    }
  display.setCursor(10, 24);
  display.print("T11: ");// Start at top-left corner
  display.print(data);
  display.display();
  delay (timeBetweenReadings);

  Serial.print("Sensor 12(*C): ");
  data = sensors.getTempC(sensor12);
  Serial.println(data);
  if (data != 85 || data != -127)
    {
      ThingSpeak.writeField(Channel5, (4), data, myWriteAPIKey5); 
    }
  display.setCursor(10, 32);
  display.print("T12: ");// Start at top-left corner
  display.print(data);
  display.display();
  delay (timeBetweenReadings);

  Serial.print("Sensor 13(*C): ");
  data = sensors.getTempC(sensor13);
  Serial.println(data);
  if (data != 85 || data != -127)
    {
      ThingSpeak.writeField(Channel5, (5), data, myWriteAPIKey5); 
    }
  display.setCursor(10, 40);
  display.print("T13: ");// Start at top-left corner
  display.print(data);
  display.display();
  delay (timeBetweenReadings);

  Serial.print("Sensor 14(*C): ");
  data = sensors.getTempC(sensor14);
  Serial.println(data);
  if (data != 85 || data != -127)
    {
      ThingSpeak.writeField(Channel5, (6), data, myWriteAPIKey5); 
    }
  display.setCursor(10, 48);
  display.print("T14: ");// Start at top-left corner
  display.print(data);
  display.display();

  delay (delayForNewReadingCycle);
}
