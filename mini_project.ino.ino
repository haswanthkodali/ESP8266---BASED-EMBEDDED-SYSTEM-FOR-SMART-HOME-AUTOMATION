#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

// 1. Put your home Wi-Fi details here:
#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// 2. Your Firebase Castle address and secret key:
#define FIREBASE_HOST "https://home-automation-faba1-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "PASTE_YOUR_DATABASE_SECRET_FROM_STEP_3A_HERE"

// 3. The 4 Pins for your 4 Relays:
#define RELAY1_PIN D1 // Light
#define RELAY2_PIN D2 // Bulb
#define RELAY3_PIN D5 // Fan
#define RELAY4_PIN D6 // Cooling Fan

// Most relays turn ON with LOW and OFF with HIGH:
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(115200);

  // Set up the 4 pins and keep them OFF safely at the start:
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);

  digitalWrite(RELAY1_PIN, RELAY_OFF);
  digitalWrite(RELAY2_PIN, RELAY_OFF);
  digitalWrite(RELAY3_PIN, RELAY_OFF);
  digitalWrite(RELAY4_PIN, RELAY_OFF);

  // Connect to Wi-Fi:
  Serial.print("Robot connecting to Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected! Yay!");
  Serial.print("Robot IP: ");
  Serial.println(WiFi.localIP());

  // Connect to Firebase:
  config.database_url = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Tell your website that the robot is awake!
  Firebase.setBool(fbdo, "/home/espOnline", true);
}

void loop() {
  // Check Light (Relay 1):
  if (Firebase.getBool(fbdo, "/home/relay1")) {
    bool state = fbdo.boolData();
    digitalWrite(RELAY1_PIN, state ? RELAY_ON : RELAY_OFF);
  }

  // Check Bulb (Relay 2):
  if (Firebase.getBool(fbdo, "/home/relay2")) {
    bool state = fbdo.boolData();
    digitalWrite(RELAY2_PIN, state ? RELAY_ON : RELAY_OFF);
  }

  // Check Fan (Relay 3):
  if (Firebase.getBool(fbdo, "/home/relay3")) {
    bool state = fbdo.boolData();
    digitalWrite(RELAY3_PIN, state ? RELAY_ON : RELAY_OFF);
  }

  // Check Cooling Fan (Relay 4):
  if (Firebase.getBool(fbdo, "/home/relay4")) {
    bool state = fbdo.boolData();
    digitalWrite(RELAY4_PIN, state ? RELAY_ON : RELAY_OFF);
  }

  // Tiny rest before checking again:
  delay(250);
}