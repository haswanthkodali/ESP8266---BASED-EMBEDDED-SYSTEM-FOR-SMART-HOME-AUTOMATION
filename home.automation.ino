#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

// 1. Put your Home Wi-Fi details here:
#define WIFI_SSID "YOUR_WIFI_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// 2. Your Firebase Castle Address & Web API Key:
#define FIREBASE_HOST "https://home-automation-faba1-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "AIzaSyBA_vMXVHsE-qB51D8osrv0piMnGGFo_cI"

// 3. The 4 Pins using real numbers (No more D1 error!):
#define RELAY1_PIN 5  // D1 on board (Light)
#define RELAY2_PIN 4  // D2 on board (Bulb)
#define RELAY3_PIN 14 // D5 on board (Fan)
#define RELAY4_PIN 12 // D6 on board (Cooling Fan)

// Most relays turn ON with LOW (0) and OFF with HIGH (1)
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(115200);

  // Set up the 4 pins and start with everything safely OFF
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  pinMode(RELAY4_PIN, OUTPUT);

  digitalWrite(RELAY1_PIN, RELAY_OFF);
  digitalWrite(RELAY2_PIN, RELAY_OFF);
  digitalWrite(RELAY3_PIN, RELAY_OFF);
  digitalWrite(RELAY4_PIN, RELAY_OFF);

  // Connect to your Wi-Fi router
  Serial.print("Robot connecting to Wi-Fi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Connected!");
  Serial.print("ESP8266 IP Address: ");
  Serial.println(WiFi.localIP());

  // Connect to Firebase
  config.database_url = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Tell your website that the robot is ONLINE!
  Firebase.setBool(fbdo, "/home/espOnline", true);
}

void loop() {
  // Check Light (Relay 1)
  if (Firebase.getBool(fbdo, "/home/relay1")) {
    bool state = fbdo.boolData();
    digitalWrite(RELAY1_PIN, state ? RELAY_ON : RELAY_OFF);
  }

  // Check Bulb (Relay 2)
  if (Firebase.getBool(fbdo, "/home/relay2")) {
    bool state = fbdo.boolData();
    digitalWrite(RELAY2_PIN, state ? RELAY_ON : RELAY_OFF);
  }

  // Check Fan (Relay 3)
  if (Firebase.getBool(fbdo, "/home/relay3")) {
    bool state = fbdo.boolData();
    digitalWrite(RELAY3_PIN, state ? RELAY_ON : RELAY_OFF);
  }

  // Check Cooling Fan (Relay 4)
  if (Firebase.getBool(fbdo, "/home/relay4")) {
    bool state = fbdo.boolData();
    digitalWrite(RELAY4_PIN, state ? RELAY_ON : RELAY_OFF);
  }

  // Tiny rest before checking again
  delay(250);
} 
