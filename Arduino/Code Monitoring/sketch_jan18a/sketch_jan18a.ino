#include <WiFi.h>
#include <PubSubClient.h>
#include "EmonLib.h"

// ---- Configuration MQTT ----
const char* mqtt_server = "192.168.200.236";  // Adresse IP de l'ESP32 Gateway (Carte 2)
const char* mqtt_topic = "energy_monitoring";
const char* alert_topic = "energy_alerts";

// ---- Configuration WiFi ----
const char* ssid = "Redmi";
const char* password = "#12345687;";

// ---- Energy Monitoring ----
EnergyMonitor emon;
#define V_CAL 106.8
#define I_CAL 0.52
float Vrms = 0.0;
float Irms = 0.0;
float power = 0.0;

// ---- Soft Starter Configuration ----
#define TRIAC_PIN 5  // Pin de commande du TRIAC
#define MIN_VOLTAGE 200.0
#define MAX_VOLTAGE 240.0
#define MAX_CURRENT 10.0

// ---- MQTT Client ----
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  // WiFi Setup
  connectWiFi();

  // MQTT Setup
  client.setServer(mqtt_server, 1883);

  // Energy Monitoring Setup
  emon.voltage(35, V_CAL, 1.7);
  emon.current(34, I_CAL);

  // Soft Starter Setup
  pinMode(TRIAC_PIN, OUTPUT);
  digitalWrite(TRIAC_PIN, LOW); // Soft Starter désactivé au démarrage

  Serial.println("Setup terminé.");
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  // Lire les données d'énergie
  emon.calcVI(20, 2000);
  Vrms = emon.Vrms;
  Irms = emon.Irms;
  power = emon.apparentPower;

  // Vérifier les anomalies
  checkForAnomalies(Vrms, Irms);

  // Publier les données via MQTT
  publishEnergyData();

  delay(5000); // Intervalle d'envoi
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

// ---- Fonction : Reconnexion MQTT ----
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connexion à MQTT...");
    if (client.connect("ESP32_Monitoring")) {
      Serial.println("Connecté à MQTT.");
    } else {
      Serial.print("Échec, rc=");
      Serial.print(client.state());
      Serial.println(" Réessayer dans 5 secondes...");
      delay(5000);
    }
  }
}

// ---- Fonction : Publier les données d'énergie ----
void publishEnergyData() {
  String payload = "{";
  payload += "\"Voltage\":" + String(Vrms) + ",";
  payload += "\"Current\":" + String(Irms) + ",";
  payload += "\"Power\":" + String(power);
  payload += "}";

  client.publish(mqtt_topic, payload.c_str());
  Serial.println("Données publiées : " + payload);
}

// ---- Fonction : Contrôle du Soft Starter ----
void controlSoftStarter(bool state) {
  if (state) {
    for (int i = 0; i <= 255; i += 5) {
      analogWrite(TRIAC_PIN, i); // Démarrage progressif
      delay(100);
    }
  } else {
    digitalWrite(TRIAC_PIN, LOW); // Arrêt
    Serial.println("Soft Starter désactivé.");
  }
}

// ---- Fonction : Vérification des anomalies ----
void checkForAnomalies(float voltage, float current) {
  if (voltage < MIN_VOLTAGE || voltage > MAX_VOLTAGE || current > MAX_CURRENT) {
    Serial.println("ALERTE : Valeurs hors plage détectées !");
    
    // Désactiver le Soft Starter pour protéger les appareils
    controlSoftStarter(false);

    // Créer une alerte JSON
    String alertPayload = "{";
    alertPayload += "\"Alert\":\"Anomalie détectée\",";
    alertPayload += "\"Voltage\":" + String(voltage) + ",";
    alertPayload += "\"Current\":" + String(current);
    alertPayload += "}";

    // Publier l'alerte via MQTT
    client.publish(alert_topic, alertPayload.c_str());
    Serial.println("Alerte publiée : " + alertPayload);
  } else {
    // Activer le Soft Starter si les valeurs sont normales
    controlSoftStarter(true);
  }
}
