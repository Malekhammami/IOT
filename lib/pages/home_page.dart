import 'package:flutter/material.dart';
import 'package:mqtt_client/mqtt_client.dart';
import '../services/mqtt_service.dart';
import '../services/sensor_data_service.dart';
import '../models/sensor_data_model.dart';
import '../services/notification_service.dart';
import 'device_page.dart';
import 'settings_page.dart';

class HomePage extends StatefulWidget {
  final NotificationService notificationService;

  const HomePage({Key? key, required this.notificationService}) : super(key: key);

  @override
  _HomePageState createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  late MQTTService mqttService;
  late SensorDataService sensorDataService;
  List<SensorData> sensorData = [];

  @override
  void initState() {
    super.initState();

    // Initialiser les services MQTT et des capteurs
    mqttService = MQTTService('broker.hivemq.com', 'flutter_client');
    sensorDataService = SensorDataService();

    // Se connecter au broker MQTT
    mqttService.connect();

    // S'abonner au topic MQTT
    mqttService.subscribe('iot/sensors', MqttQos.atLeastOnce);
    mqttService.client.updates!.listen((List<MqttReceivedMessage<MqttMessage>> messages) {
      final MqttPublishMessage message = messages[0].payload as MqttPublishMessage;
      final String payload = MqttPublishPayload.bytesToStringAsString(message.payload.message);

      // Traiter les données reçues
      final parsedData = sensorDataService.parseSensorData(payload);
      setState(() {
        sensorData = parsedData;
      });

      // Envoyer une notification si une valeur critique est détectée
      if (parsedData.any((data) => data.value > 50.0)) {
        widget.notificationService.showNotification(
          'Alerte Capteur',
          'Une valeur critique a été détectée !',
        );
      }
    });
  }

  @override
  void dispose() {
    mqttService.disconnect();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Dashboard IoT'),
        actions: [
          IconButton(
            icon: const Icon(Icons.settings),
            onPressed: () {
              Navigator.push(context, MaterialPageRoute(builder: (_) => const SettingsPage()));
            },
          ),
        ],
      ),
      body: sensorData.isEmpty
          ? const Center(child: CircularProgressIndicator())
          : ListView.builder(
        itemCount: sensorData.length,
        itemBuilder: (context, index) {
          final data = sensorData[index];
          return Card(
            child: ListTile(
              title: Text(data.name),
              subtitle: Text('Valeur: ${data.value}'),
              trailing: Text(data.timestamp.toLocal().toString()),
              onTap: () {
                Navigator.push(
                  context,
                  MaterialPageRoute(
                    builder: (_) => DevicePage(deviceName: data.name),
                  ),
                );
              },
            ),
          );
        },
      ),
    );
  }
}
