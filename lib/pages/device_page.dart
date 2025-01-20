import 'package:flutter/material.dart';
import '../widgets/sensor_chart.dart';
import '../services/device_control_service.dart';

class DevicePage extends StatefulWidget {
  final String deviceName;

  const DevicePage({Key? key, required this.deviceName}) : super(key: key);

  @override
  _DevicePageState createState() => _DevicePageState();
}

class _DevicePageState extends State<DevicePage> {
  late DeviceControlService deviceControlService;

  @override
  void initState() {
    super.initState();
    deviceControlService = DeviceControlService();
  }

  void showSnackBar(String message) {
    ScaffoldMessenger.of(context).showSnackBar(
      SnackBar(content: Text(message)),
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(widget.deviceName),
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: [
            Expanded(
              child: SensorChart(sensorName: '${widget.deviceName} Sensor'),
            ),
            const SizedBox(height: 20),
            ElevatedButton(
              onPressed: () {
                deviceControlService.turnOnDevice(widget.deviceName);
                showSnackBar('${widget.deviceName} allumé.');
              },
              child: const Text('Allumer Appareil'),
            ),
            ElevatedButton(
              onPressed: () {
                deviceControlService.turnOffDevice(widget.deviceName);
                showSnackBar('${widget.deviceName} éteint.');
              },
              child: const Text('Éteindre Appareil'),
            ),
            ElevatedButton(
              onPressed: () {
                deviceControlService.sendCommand(widget.deviceName, 'Réinitialiser');
                showSnackBar('Commande "Réinitialiser" envoyée à ${widget.deviceName}.');
              },
              child: const Text('Réinitialiser Appareil'),
            ),
          ],
        ),
      ),
    );
  }
}
