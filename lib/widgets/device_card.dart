import 'package:flutter/material.dart';

class DeviceCard extends StatelessWidget {
  final String name;
  final String status;
  final VoidCallback onTap;

  const DeviceCard({Key? key, required this.name, required this.status, required this.onTap}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Card(
      child: ListTile(
        title: Text(name),
        subtitle: Text('Status: $status'),
        onTap: onTap,
      ),
    );
  }
}
