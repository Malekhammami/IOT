import 'package:mqtt_client/mqtt_client.dart';
import 'package:mqtt_client/mqtt_server_client.dart';

class MQTTService {
  final String broker;
  final String clientId;
  late MqttServerClient client;

  MQTTService(this.broker, this.clientId);

  Future<void> connect() async {
    client = MqttServerClient(broker, clientId);
    client.logging(on: true);
    client.keepAlivePeriod = 20;

    // Configuration des callbacks
    client.onConnected = onConnected;
    client.onDisconnected = onDisconnected;
    client.onSubscribed = onSubscribed;
    client.onSubscribeFail = onSubscribeFail;

    final connMessage = MqttConnectMessage()
        .withClientIdentifier(clientId)
        .withWillTopic('willtopic') // Facultatif
        .withWillMessage('Will message')
        .startClean()
        .withWillQos(MqttQos.atLeastOnce);
    client.connectionMessage = connMessage;

    try {
      await client.connect();
    } catch (e) {
      print('Erreur lors de la connexion : $e');
      client.disconnect();
    }
  }

  void onConnected() {
    print('Connecté au broker MQTT');
  }

  void onDisconnected() {
    print('Déconnecté du broker MQTT');
  }

  void onSubscribed(String topic) {
    print('Abonné au topic : $topic');
  }

  void onSubscribeFail(String topic) {
    print('Échec de l’abonnement au topic : $topic');
  }

  void subscribe(String topic, MqttQos qos) {
    if (client.connectionStatus!.state == MqttConnectionState.connected) {
      client.subscribe(topic, MqttQos.atMostOnce);
      client.updates!.listen((List<MqttReceivedMessage<MqttMessage>> messages) {
        final MqttReceivedMessage<MqttMessage> receivedMessage = messages[0];
        final MqttPublishMessage payloadMessage = receivedMessage.payload as MqttPublishMessage;
        final String payload = MqttPublishPayload.bytesToStringAsString(payloadMessage.payload.message);

        print('Message reçu sur $topic : $payload');
      });
    } else {
      print('Client non connecté. Impossible de s’abonner.');
    }
  }

  void disconnect() {
    client.disconnect();
    print('Déconnecté');
  }
}
