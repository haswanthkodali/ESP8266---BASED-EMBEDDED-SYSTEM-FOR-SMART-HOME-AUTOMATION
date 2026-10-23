#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <DHT.h>

// 1. Your Home Wi-Fi:
#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// 2. Your Firebase Castle details:
#define FIREBASE_HOST "https://home-automation-faba1-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "AIzaSyBA_vMXVHsE-qB51D8osrv0piMnGGFo_cI"

// 3. Relay Pins:
#define RELAY1_PIN 5  // D1 on board (Light)
#define RELAY2_PIN 4  // D2 on board (Bulb)
#define RELAY3_PIN 14 // D5 on board (Fan)
#define RELAY4_PIN 12 // D6 on board (Cooling Fan)

// 4. Temperature Sensor Pin:
#define DHTPIN 13     // D7 on board
#define DHTTYPE DHT11 // Change to DHT22 if using white sensor

DHT dht(DHTPIN, DHTTYPE);

#define RELAY_ON  LOW
#define RELAY_OFF HIGH

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long lastSensorTime = 0;
bool autoFanTriggered = false;

void setup() {
  Serial.begin(115200);

  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);

  digitalWrite(RELAY1_PIN, RELAY_OFF);
  digitalWrite(RELAY2_PIN, RELAY_OFF);
  digitalWrite(RELAY3_PIN, RELAY_OFF);
  digitalWrite(RELAY4_PIN, RELAY_OFF);

  dht.begin();

  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected!");

  config.database_url = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Firebase.setBool(fbdo, "/home/espOnline", true);
}

void loop() {
  // ─── 1. Check Webpage Commands ───
  if (Firebase.getBool(fbdo, "/home/relay1")) {
    digitalWrite(RELAY1_PIN, fbdo.boolData() ? RELAY_ON : RELAY_OFF);
  }
  if (Firebase.getBool(fbdo, "/home/relay2")) {
    digitalWrite(RELAY2_PIN, fbdo.boolData() ? RELAY_ON : RELAY_OFF);
  }
  if (Firebase.getBool(fbdo, "/home/relay3")) {
    digitalWrite(RELAY3_PIN, fbdo.boolData() ? RELAY_ON : RELAY_OFF);
  }
  if (Firebase.getBool(fbdo, "/home/relay4")) {
    digitalWrite(RELAY4_PIN, fbdo.boolData() ? RELAY_ON : RELAY_OFF);
  }

  // ─── 2. Sensor Check & Auto-Cooling at >40°C ───
  if (millis() - lastSensorTime >= 2000) {
    lastSensorTime = millis();

    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (!isnan(temp) && !isnan(hum)) {
      Serial.print("Temp: ");
      Serial.print(temp);
      Serial.println(" °C");

      // Update temperature and humidity in Firebase
      Firebase.setFloat(fbdo, "/home/temperature", temp);
      Firebase.setFloat(fbdo, "/home/humidity", hum);

      // 🔥 AUTO-COOLING RULE: More than 40°C!
      if (temp > 40.0) {
        if (!autoFanTriggered) {
          autoFanTriggered = true;
          Serial.println("🔥 >40°C Detected! Turning ON Cooling Fan!");
          
          // 1. Physically turn ON Cooling Fan
          digitalWrite(RELAY4_PIN, RELAY_ON);
          
          // 2. Update Firebase so webpage shows it ON!
          Firebase.setBool(fbdo, "/home/relay4", true);
          Firebase.setBool(fbdo, "/home/autoCoolingActive", true);
        }
      } else if (temp <= 38.0 && autoFanTriggered) {
        // Cooled down safely below 38°C
        autoFanTriggered = false;
        Firebase.setBool(fbdo, "/home/autoCoolingActive", false);
      }
    }
  }

  delay(200);
}
