#define BLYNK_TEMPLATE_ID "TMPL2a_I8q5Gd"
#define BLYNK_TEMPLATE_NAME "IOT"
#define BLYNK_AUTH_TOKEN "940xdA-1ObVLZfdALj2JfHmP44ijN0W7"

#include <WiFi.h>
#include <FirebaseESP32.h>
#include <PubSubClient.h>
#include <BlynkSimpleEsp32.h>
#include <ArduinoJson.h>

// ---- Configuration WiFi ----
const char* ssid = "Redmi";
const char* password = "#12345687;";

// ---- Configuration Firebase ----
#define FIREBASE_HOST "https://iot-d4e50-default-rtdb.firebaseio.com"
#define FIREBASE_API_KEY "AIzaSyCskB3XK4W5QItYDnW4sF1d6Y2JE0pcgJ4"
#define USER_EMAIL "malekhammami82@gmail.com"
#define USER_PASSWORD "Abcdef12@"

FirebaseData firebaseData;
FirebaseAuth firebaseAuth;
FirebaseConfig firebaseConfig;

// ---- Configuration MQTT ----
const char* mqtt_server = "192.168.200.236";
const char* mqtt_topic = "energy_monitoring";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  // Initialisation WiFi
  connectWiFi();

  // Initialisation Firebase
  initFirebase();

  // Initialisation Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  // Initialisation MQTT
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Serial.println("Setup terminé.");
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();
  Blynk.run();
}

// ---- Fonction : Connexion WiFi ----
void connectWiFi() {
  Serial.print("Connexion au WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connecté.");
}

// ---- Fonction : Initialisation Firebase ----
void initFirebase() {
  firebaseConfig.api_key = FIREBASE_API_KEY;
  firebaseConfig.database_url = FIREBASE_HOST;
  firebaseAuth.user.email = USER_EMAIL;
  firebaseAuth.user.password = USER_PASSWORD;

  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);

  if (Firebase.ready()) {
    Serial.println("Firebase connecté avec succès.");
  } else {
    Serial.print("Erreur Firebase : ");
    Serial.println(firebaseConfig.signer.tokens.error.message.c_str());
  }
}

// ---- Fonction : Reconnexion MQTT ----
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connexion à MQTT...");
    if (client.connect("ESP32_Gateway")) {
      client.subscribe(mqtt_topic);
      Serial.println("Connecté à MQTT.");
    } else {
      Serial.print("Échec, rc=");
      Serial.print(client.state());
      Serial.println(" Réessayer dans 5 secondes...");
      delay(5000);
    }
  }
}

// ---- Callback MQTT ----
void callback(char* topic, byte* payload, unsigned int length) {
  String data = "";
  for (int i = 0; i < length; i++) {
    data += (char)payload[i];
  }

  Serial.println("Données reçues via MQTT : " + data);

  // Envoyer les données à Firebase
  if (Firebase.ready()) {
    if (Firebase.setString(firebaseData, "/Monitoring", data)) {
      Serial.println("Données envoyées à Firebase avec succès.");
    } else {
      Serial.print("Échec d'envoi des données à Firebase : ");
      Serial.println(firebaseData.errorReason());
    }
  } else {
    Serial.println("Firebase non prêt.");
  }

  // Analyse des données JSON
  processJsonData(data);
}

// ---- Fonction : Analyse des données JSON ----
void processJsonData(const String& data) {
  DynamicJsonDocument doc(2048); // Taille ajustée pour les données potentielles
  DeserializationError error = deserializeJson(doc, data);

  if (!error) {
    float voltage = doc["Voltage"] | 0.0;  // Valeur par défaut si clé manquante
    float current = doc["Current"] | 0.0;
    float power = doc["Power"] | 0.0;

    // Envoyer les données à Blynk
    Blynk.virtualWrite(V0, voltage);
    Blynk.virtualWrite(V1, current);
    Blynk.virtualWrite(V2, power);

    Serial.println("Données envoyées à Blynk.");
  } else {
    Serial.print("Erreur de parsing JSON : ");
    Serial.println(error.c_str());
  }
}
