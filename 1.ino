#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const char* ssid = "HUAWEI-C5D8";
const char* password = "T4TB36H3FTM";

#define DATABASE_URL "https://smartfridgemonitor-default-rtdb.firebaseio.com/"
#define DATABASE_SECRET "PqPaeV410pEEdEfUxLsfy2gBbdg0L7zFCfj3HCcB"

#define MQ135_PIN 34
#define MQ4_PIN 35
#define DHT_PIN 4
#define BUZZER_PIN 13
#define DHT_TYPE DHT22

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

DHT dht(DHT_PIN, DHT_TYPE);

int mq135_value = 0;
int mq4_value = 0;
float temperature = 0;
float humidity = 0;
bool demoMode = false;

int mq135_threshold = 2000;
int mq4_threshold = 1500;
float temp_threshold = 40.0;
float hum_threshold = 90.0;
bool alert_status = false;

unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 2000;

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n=== FreshGuard Starting ===");
  
  initializeOLED();
  initializeSensors();
  connectToWiFi();
  setupFirebase();
  setDefaultThresholds();
  
  Serial.println("=== Setup Complete ===");
}

void loop() {
  unsigned long currentMillis = millis();
  
  if (currentMillis - lastUpdate >= UPDATE_INTERVAL) {
    readSensors();
    checkThresholds();
    updateFirebase();
    updateDisplay();
    lastUpdate = currentMillis;
  }
}