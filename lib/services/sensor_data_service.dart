import '../models/sensor_data_model.dart';

class SensorDataService {
  List<SensorData> parseSensorData(String rawData) {
    // Simuler le traitement des données
    return [
      SensorData(name: 'Temperature', value: 22.5, timestamp: DateTime.now()),
      SensorData(name: 'Humidity', value: 45.0, timestamp: DateTime.now()),
    ];
  }

  double calculateAverage(List<SensorData> data) {
    final sum = data.fold(0.0, (previous, current) => previous + current.value);
    return sum / data.length;
  }
}
