import 'package:flutter_local_notifications/flutter_local_notifications.dart';


class NotificationService {
  final FlutterLocalNotificationsPlugin notificationsPlugin = FlutterLocalNotificationsPlugin();

  Future<void> initialize() async {
    const android = AndroidInitializationSettings('@mipmap/ic_launcher');
    const initializationSettings = InitializationSettings(android: android);
    await notificationsPlugin.initialize(initializationSettings);
  }

  Future<void> showNotification(String title, String body) async {
    const androidDetails = AndroidNotificationDetails(
      'channel_id',
      'channel_name',
      importance: Importance.high,
      priority: Priority.high,
    );
    const notificationDetails = NotificationDetails(android: androidDetails);
    await notificationsPlugin.show(0, title, body, notificationDetails);
    print('Notification shown: $title - $body');
  }
}
class MockNotificationService extends NotificationService {
  @override
  Future<void> initialize() async {
    // Simule l'initialisation sans actions réelles
    print('Mock NotificationService initialized');
  }

  @override
  Future<void> showNotification(String title, String body) async {
    // Simule l'affichage de la notification
    print('Mock notification shown: $title - $body');
  }
}

