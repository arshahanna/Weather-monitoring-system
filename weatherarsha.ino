#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>
#include <DHT.h>

// --- WiFi and ThingSpeak Setup ---
const char* ssid = "CMF BY NOTHING Phone 1_9747";      // your WiFi name
const char* password = "ninakenthinaa";                // your WiFi password
unsigned long myChannelNumber = 3121593;
const char* myWriteAPIKey = "CA1WVVT7OOI4U73P";        // from ThingSpeak

WiFiClient client;

// --- Sensor Setup ---
#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Initialize BMP sensor
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1);
  }

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  
  ThingSpeak.begin(client);
}

void loop() {
  // --- Read sensors ---
  float tempDHT = dht.readTemperature();
  float humDHT = dht.readHumidity();

  sensors_event_t event;
  bmp.getEvent(&event);
  float pressureBMP = event.pressure; // pressure in hPa

  float tempBMP;
  bmp.getTemperature(&tempBMP);       // get temperature in °C

  // --- Print to Serial ---
  Serial.println("---- WEATHER STATION ----");
  Serial.print("Temp (DHT11): "); Serial.print(tempDHT); Serial.println(" °C");
  Serial.print("Humidity: "); Serial.print(humDHT); Serial.println(" %");
  Serial.print("Temp (BMP180): "); Serial.print(tempBMP); Serial.println(" °C");
  Serial.print("Pressure: "); Serial.print(pressureBMP); Serial.println(" hPa");

  // --- Send data to ThingSpeak ---
  ThingSpeak.setField(1, tempDHT);
  ThingSpeak.setField(2, humDHT);
  ThingSpeak.setField(3, tempBMP);
  ThingSpeak.setField(4, pressureBMP);

  int response = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (response == 200) {
    Serial.println("Data sent successfully!");
  } else {
    Serial.print("Error sending data. Code: ");
    Serial.println(response);
  }

  delay(20000); // ThingSpeak minimum update interval is 15 seconds
}
