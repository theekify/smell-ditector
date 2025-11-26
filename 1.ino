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
  
  display.print("Time: ");
  display.print(millis() / 1000);
  display.println("s");
  
  display.setTextSize(2);
  if (alert_status) {
    display.println(" ALERT!");
  } else {
    display.println(" NORMAL");
  }
  
  display.display();
}