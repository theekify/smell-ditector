void readSensors() {
  Serial.println("\n--- Reading Sensors ---");
  
  mq135_value = analogRead(MQ135_PIN);
  mq4_value = analogRead(MQ4_PIN);
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

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

  unsigned long currentTime = millis() / 1000;

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

  if (Firebase.RTDB.setInt(&fbdo, "/status/lastUpdate", currentTime)) {
    Serial.print("Timestamp updated: ");
    Serial.println(currentTime);
  } else {
    Serial.print("Timestamp update failed: ");
    Serial.println(fbdo.errorReason().c_str());
  }

  if (Firebase.RTDB.setBool(&fbdo, "/status/alert", alert_status)) {
    Serial.println("Alert status updated");
  } else {
    Serial.print("Alert status update failed: ");
    Serial.println(fbdo.errorReason().c_str());
  }

  if (Firebase.RTDB.setBool(&fbdo, "/status/online", true)) {
    Serial.println("Online status updated");
  } else {
    Serial.print("Online status update failed: ");
    Serial.println(fbdo.errorReason().c_str());
  }
}