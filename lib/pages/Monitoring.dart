import 'package:flutter/material.dart';
import 'package:iot_project/services/mqtt_service.dart';
import 'package:iot_project/services/firebase_service.dart';
import 'package:cloud_firestore/cloud_firestore.dart';
import 'package:mqtt_client/mqtt_client.dart';

class MonitoringPage extends StatefulWidget {
  @override
  _MonitoringPageState createState() => _MonitoringPageState();
}

class _MonitoringPageState extends State<MonitoringPage> {
  late MQTTService mqttService;
  late FirebaseService firebaseService;

  @override
  void initState() {
    super.initState();

    mqttService = MQTTService('192.168.200.236', 'flutter_client');
    firebaseService = FirebaseService();

    mqttService.connect().then((_) {
      mqttService.subscribe('energy_monitoring', (message) {
        print('Message reçu : $message');
        // Vous pouvez également mettre à jour l'interface utilisateur ici
      } as MqttQos);
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
        title: Text('Monitoring IoT'),
      ),
      body: StreamBuilder<QuerySnapshot>(
        stream: firebaseService.listenToCollection('Monitoring'),
        builder: (context, snapshot) {
          if (snapshot.connectionState == ConnectionState.waiting) {
            return Center(child: CircularProgressIndicator());
          }

          if (snapshot.hasError) {
            return Center(child: Text('Erreur de chargement des données'));
          }

          final data = snapshot.data?.docs ?? [];

          return ListView.builder(
            itemCount: data.length,
            itemBuilder: (context, index) {
              final doc = data[index];
              final monitoringData = doc.data() as Map<String, dynamic>;

              return Card(
                child: ListTile(
                  title: Text('Voltage: ${monitoringData['Voltage']} V'),
                  subtitle: Text('Current: ${monitoringData['Current']} A'),
                  trailing: Text('Power: ${monitoringData['Power']} W'),
                ),
              );
            },
          );
        },
      ),
    );
  }
}
