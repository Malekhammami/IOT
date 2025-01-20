import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';

import 'package:iot_project/main.dart';
import 'package:iot_project/services/notification_service.dart';

void main() {
  testWidgets('Counter increments smoke test', (WidgetTester tester) async {
    // Utiliser un mock du service de notifications
    final notificationService = MockNotificationService();

    // Build l'application avec le mock
    await tester.pumpWidget(MyApp(notificationService: notificationService));

    // Vérifier qu'un texte '0' est visible
    expect(find.text('0'), findsOneWidget);
    expect(find.text('1'), findsNothing);

    // Simuler un tap sur le bouton "+"
    await tester.tap(find.byIcon(Icons.add));
    await tester.pump();

    // Vérifier qu'un texte '1' est visible
    expect(find.text('0'), findsNothing);
    expect(find.text('1'), findsOneWidget);
  });
}
