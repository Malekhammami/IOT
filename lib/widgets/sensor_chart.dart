import 'package:flutter/material.dart';

class SensorChart extends StatelessWidget {
  final String sensorName;

  const SensorChart({Key? key, required this.sensorName}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Column(
      mainAxisAlignment: MainAxisAlignment.center,
      children: [
        Text(sensorName, style: const TextStyle(fontSize: 20, fontWeight: FontWeight.bold)),
        const SizedBox(height: 20),
        const Placeholder(fallbackHeight: 200, fallbackWidth: double.infinity),
      ],
    );
  }
}
