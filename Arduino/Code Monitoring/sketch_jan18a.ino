// ---- Définir les identifiants de votre projet Blynk ----
#define BLYNK_TEMPLATE_ID "TMPL2a_I8q5Gd"  // ID du modèle Blynk
#define BLYNK_TEMPLATE_NAME "IOT"          // Nom du modèle Blynk
#define BLYNK_AUTH_TOKEN "940xdA-1ObVLZfdALj2JfHmP44ijN0W7"  // Token d'authentification Blynk

// ---- Inclure les bibliothèques nécessaires ----
#include <WiFi.h>               // Gestion de la connexion WiFi
#include <FirebaseESP32.h>      // Gestion des interactions avec Firebase
#include <PubSubClient.h>       // Gestion du protocole MQTT
#include <BlynkSimpleEsp32.h>   // Intégration avec Blynk
#include <ArduinoJson.h>        // Gestion et parsing des données JSON

// ---- Configuration WiFi ----
const char* ssid = "Redmi";         // Nom du réseau WiFi
const char* password = "#12345687;"; // Mot de passe du réseau WiFi

// ---- Configuration Firebase ----
#define FIREBASE_HOST "https://iot-d4e50-default-rtdb.firebaseio.com"  // URL de la base de données Firebase
#define FIREBASE_API_KEY "AIzaSyCskB3XK4W5QItYDnW4sF1d6Y2JE0pcgJ4"     // Clé API Firebase
#define USER_EMAIL "malekhammami82@gmail.com"                         // Email utilisateur pour Firebase
#define USER_PASSWORD "Abcdef12@"                                     // Mot de passe utilisateur pour Firebase

FirebaseData firebaseData;  // Objet pour interagir avec Firebase
FirebaseAuth firebaseAuth;  // Authentification Firebase
FirebaseConfig firebaseConfig; // Configuration Firebase

// ---- Configuration MQTT ----
const char* mqtt_server = "192.168.200.236"; // Adresse IP du serveur MQTT
const char* mqtt_topic = "energy_monitoring"; // Topic pour publier et souscrire
WiFiClient espClient;           // Client WiFi pour MQTT
PubSubClient client(espClient); // Client MQTT

// ---- Fonction d'initialisation ----
void setup() {
  Serial.begin(115200); // Initialisation de la communication série

  // Connexion au réseau WiFi
  connectWiFi();

  // Configuration de Firebase
  firebaseConfig.api_key = FIREBASE_API_KEY;       // Définir la clé API
  firebaseConfig.database_url = FIREBASE_HOST;     // Définir l'URL de la base de données
  firebaseAuth.user.email = USER_EMAIL;           // Définir l'email utilisateur
  firebaseAuth.user.password = USER_PASSWORD;     // Définir le mot de passe utilisateur
  Firebase.begin(&firebaseConfig, &firebaseAuth); // Démarrer Firebase
  Firebase.reconnectWiFi(true);                   // Reconnexion automatique si WiFi est perdu

  if (Firebase.ready()) {
    Serial.println("Firebase connecté avec succès."); // Confirmation de la connexion Firebase
  } else {
    Serial.print("Erreur Firebase : ");
    Serial.println(firebaseConfig.signer.tokens.error.message.c_str());
  }

  // Configuration Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);

  // Configuration MQTT
  client.setServer(mqtt_server, 1883);    // Définir le serveur MQTT
  client.setCallback(callback);          // Définir la fonction de rappel pour les messages MQTT

  Serial.println("Setup terminé.");
}

// ---- Boucle principale ----
void loop() {
  if (!client.connected()) {   // Vérifier la connexion MQTT
    reconnectMQTT();           // Reconnexion si nécessaire
  }
  client.loop();  // Maintenir la connexion MQTT active
  Blynk.run();    // Maintenir la connexion Blynk active
}

// ---- Connexion WiFi ----
void connectWiFi() {
  Serial.print("Connexion au WiFi...");
  WiFi.begin(ssid, password);   // Démarrer la connexion WiFi
  while (WiFi.status() != WL_CONNECTED) { // Attendre la connexion
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connecté."); // Confirmation de la connexion WiFi
}

// ---- Reconnexion MQTT ----
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connexion à MQTT...");
    if (client.connect("ESP32_Gateway")) { // Tenter de se connecter au serveur MQTT
      client.subscribe(mqtt_topic);        // Souscrire au topic
      Serial.println("Connecté à MQTT.");
    } else {
      Serial.print("Échec, rc=");
      Serial.print(client.state());
      Serial.println(" Réessayer dans 5 secondes...");
      delay(5000); // Pause avant une nouvelle tentative
    }
  }
}

// ---- Fonction de rappel MQTT ----
void callback(char* topic, byte* payload, unsigned int length) {
  // Conversion des données reçues en chaîne de caractères
  String data = "";
  for (int i = 0; i < length; i++) {
    data += (char)payload[i];
  }

  Serial.println("Données reçues via MQTT : " + data);

  // Envoi des données à Firebase
  if (Firebase.setString(firebaseData, "/Monitoring", data)) {
    Serial.println("Données envoyées à Firebase avec succès.");
  } else {
    Serial.print("Échec d'envoi des données à Firebase : ");
    Serial.println(firebaseData.errorReason());
  }

  // Analyse des données JSON
  DynamicJsonDocument doc(2048); // Taille du document JSON
  DeserializationError error = deserializeJson(doc, data); // Parsing des données

  if (!error) {
    // Extraction des valeurs JSON
    float voltage = doc["Voltage"] | 0.0;  // Valeur par défaut si clé absente
    float current = doc["Current"] | 0.0;
    float power = doc["Power"] | 0.0;

    // Envoi des données à Blynk
    Blynk.virtualWrite(V0, voltage); // Tension
    Blynk.virtualWrite(V1, current); // Courant
    Blynk.virtualWrite(V2, power);   // Puissance

    Serial.println("Données envoyées à Blynk.");
  } else {
    Serial.println("Erreur de parsing JSON."); // Erreur d'analyse JSON
  }
}
