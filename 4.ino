void setupFirebase() {
  Serial.println("Setting up Firebase...");
  
  config.database_url = DATABASE_URL;
  config.signer.tokens.legacy_token = DATABASE_SECRET;
  
  config.timeout.serverResponse = 10 * 1000;
  config.timeout.sslHandshake = 10 * 1000;
  
  Firebase.reconnectNetwork(true);
  
  Serial.print("Initializing Firebase... ");
  
  Firebase.begin(&config, &auth);
  
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