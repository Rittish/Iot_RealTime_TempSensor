#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>
#include <ArduinoJson.h>

// WiFi credentials
#define WIFI_SSID "mithun"
#define WIFI_PASSWORD "mithunzz"

// Firebase configuration
#define FIREBASE_HOST "iotrial-8e2c0-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "tqUxt4dUqQLTmWvPa629yWkjl0CfjTqybeEBy7ar"

// DHT Sensor
#define DHTPIN D1
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  // Initialize Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Read sensor data
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if readings failed
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print("%\tTemperature: ");
  Serial.print(temperature);
  Serial.println("°C");

  // Send to Firebase
  if (Firebase.RTDB.setFloat(&fbdo, "DHT/humidity", humidity)) {
    Serial.println("Humidity sent to Firebase");
  } else {
    Serial.println("Failed to send humidity");
    Serial.println(fbdo.errorReason());
  }

  if (Firebase.RTDB.setFloat(&fbdo, "DHT/temperature", temperature)) {
    Serial.println("Temperature sent to Firebase");
  } else {
    Serial.println("Failed to send temperature");
    Serial.println(fbdo.errorReason());
  }

  delay(5000); // 5 second delay between readings
}