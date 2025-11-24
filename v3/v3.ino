#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// WiFi Credentials
const char* ssid = "HUAWEI-C5D8";
const char* password = "T4TB36H3FTM";

// Firebase Config
#define DATABASE_URL "https://smartfridgemonitor-default-rtdb.firebaseio.com/"
#define DATABASE_SECRET "PqPaeV410pEEdEfUxLsfy2gBbdg0L7zFCfj3HCcB"

// Sensor Pins
#define MQ135_PIN 34
#define MQ4_PIN 35
#define DHT_PIN 4
#define BUZZER_PIN 13
#define DHT_TYPE DHT22

// OLED Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Firebase Objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Sensor Objects
DHT dht(DHT_PIN, DHT_TYPE);

// Variables
int mq135_value = 0;
int mq4_value = 0;
float temperature = 0;
float humidity = 0;
bool demoMode = false;

// Thresholds (Updated values)
int mq135_threshold = 2000;
int mq4_threshold = 1500;
float temp_threshold = 40.0;    // Changed from 8.0 to 40.0
float hum_threshold = 90.0;     // Changed from 80.0 to 90.0
bool alert_status = false;

unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 2000; // Changed from 3000 to 2000 ms

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

void initializeOLED() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED allocation failed!");
    return;
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Starting...");
  display.display();
  Serial.println("OLED initialized");
}

void initializeSensors() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  dht.begin();
  Serial.println("Sensors initialized");
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi: Connecting...");
  display.display();
  
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi CONNECTED!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("WiFi: CONNECTED");
    display.print("IP: ");
    display.println(WiFi.localIP());
    display.display();
    delay(1000);
  } else {
    Serial.println("\nWiFi FAILED!");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("WiFi: FAILED!");
    display.display();
  }
}

void setupFirebase() {
  Serial.println("Setting up Firebase...");
  
  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = DATABASE_SECRET;
  
  // Important: Add these configurations
  config.timeout.serverResponse = 10 * 1000; // 10 seconds
  config.timeout.sslHandshake = 10 * 1000; // 10 seconds
  
  Firebase.reconnectNetwork(true);
  
  Serial.print("Initializing Firebase... ");
  
  // Initialize Firebase (newer versions return void)
  Firebase.begin(&config, &auth);
  
  // Wait for Firebase to be ready
  Serial.println("Initialized");
  Serial.print("Waiting for Firebase to be ready... ");
  
  int firebaseAttempts = 0;
  while (!Firebase.ready() && firebaseAttempts < 10) {
    delay(500);
    Serial.print(".");
    firebaseAttempts++;
  }
  
  if (Firebase.ready()) {
    Serial.println("READY");
    
    // Test the connection
    Serial.print("Testing Firebase connection... ");
    if (Firebase.RTDB.setInt(&fbdo, "/test/connection", 123)) {
      Serial.println("SUCCESS");
    } else {
      Serial.println("FAILED");
      Serial.print("Error: ");
      Serial.println(fbdo.errorReason().c_str());
    }
    
  } else {
    Serial.println("NOT READY");
  }
}

void setDefaultThresholds() {
  if (Firebase.ready()) {
    Firebase.RTDB.setInt(&fbdo, "/thresholds/mq135", mq135_threshold);
    Firebase.RTDB.setInt(&fbdo, "/thresholds/mq4", mq4_threshold);
    Firebase.RTDB.setFloat(&fbdo, "/thresholds/temperature", temp_threshold);
    Firebase.RTDB.setFloat(&fbdo, "/thresholds/humidity", hum_threshold);
    Serial.println("Default thresholds set");
  }
}

void readSensors() {
  Serial.println("\n--- Reading Sensors ---");
  
  mq135_value = analogRead(MQ135_PIN);
  mq4_value = analogRead(MQ4_PIN);
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  // Handle sensor errors
  if (isnan(temperature)) {
    Serial.println("DHT22 Temperature read failed!");
    temperature = -999;
  }
  if (isnan(humidity)) {
    Serial.println("DHT22 Humidity read failed!");
    humidity = -999;
  }

  Serial.print("MQ135: "); Serial.println(mq135_value);
  Serial.print("MQ4: "); Serial.println(mq4_value);
  Serial.print("Temp: "); Serial.print(temperature); Serial.println("°C");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println("%");
}

void checkThresholds() {
  alert_status = (mq135_value > mq135_threshold) ||
                 (mq4_value > mq4_threshold) ||
                 (temperature > temp_threshold) ||
                 (humidity > hum_threshold);

  digitalWrite(BUZZER_PIN, alert_status ? HIGH : LOW);
  
  if (alert_status) {
    Serial.println("ALERT! Threshold exceeded!");
  }
}

void updateFirebase() {
  Serial.println("--- Updating Firebase ---");
  
  if (!Firebase.ready()) {
    Serial.println("Firebase not ready!");
    return;
  }

  // Get current timestamp in seconds (using millis as relative time)
  unsigned long currentTime = millis() / 1000;

  // Update sensor data
  if (Firebase.RTDB.setInt(&fbdo, "/sensors/mq135", mq135_value)) {
    Serial.println("MQ135 updated successfully");
  } else {
    Serial.print("MQ135 update failed: ");
    Serial.println(fbdo.errorReason().c_str());
  }

  if (Firebase.RTDB.setInt(&fbdo, "/sensors/mq4", mq4_value)) {
    Serial.println("MQ4 updated successfully");
  } else {
    Serial.print("MQ4 update failed: ");

    Serial.println(fbdo.errorReason().c_str());
  }

  if (Firebase.RTDB.setFloat(&fbdo, "/sensors/temperature", temperature)) {
    Serial.println("Temperature updated successfully");
  } else {
    Serial.print("Temperature update failed: ");
    Serial.println(fbdo.errorReason().c_str());
  }

  if (Firebase.RTDB.setFloat(&fbdo, "/sensors/humidity", humidity)) {
    Serial.println("Humidity updated successfully");
  } else {
    Serial.print("Humidity update failed: ");
    Serial.println(fbdo.errorReason().c_str());
  }

  // Update timestamp (relative time in seconds)
  if (Firebase.RTDB.setInt(&fbdo, "/status/lastUpdate", currentTime)) {
    Serial.print("Timestamp updated: ");
    Serial.println(currentTime);
  } else {
    Serial.print("Timestamp update failed: ");
    Serial.println(fbdo.errorReason().c_str());
  }

  // Update alert status
  if (Firebase.RTDB.setBool(&fbdo, "/status/alert", alert_status)) {
    Serial.println("Alert status updated");
  } else {
    Serial.print("Alert status update failed: ");
    Serial.println(fbdo.errorReason().c_str());
  }

  // Update online status
  if (Firebase.RTDB.setBool(&fbdo, "/status/online", true)) {
    Serial.println("Online status updated");
  } else {
    Serial.print("Online status update failed: ");
    Serial.println(fbdo.errorReason().c_str());
  }
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  display.print("MQ135: "); display.println(mq135_value);
  display.print("MQ4: "); display.println(mq4_value);
  display.print("Temp: "); 
  if (temperature == -999) {
    display.println("ERR");
  } else {
    display.print(temperature, 1); display.println("C");
  }
  display.print("Hum: "); 
  if (humidity == -999) {
    display.println("ERR");
  } else {
    display.print(humidity, 1); display.println("%");
  }
  
  display.print("WiFi: ");
  display.println(WiFi.status() == WL_CONNECTED ? "OK" : "OFF");
  
  display.print("Firebase: ");
  display.println(Firebase.ready() ? "OK" : "OFF");
  
  // Show relative time (Time feature kept)
  display.print("Time: ");
  display.print(millis() / 1000);
  display.println("s");
  
  // Alert status
  display.setTextSize(2);
  if (alert_status) {
    display.println(" ALERT!");
  } else {
    display.println(" NORMAL");
  }
  
  display.display();
}