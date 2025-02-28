#include <ArduinoJson.h>

#define LED_PIN 13        // LED d'alarme
#define SENSOR_PIN A0     // Capteur de luminosité
#define BUTTON1_PIN 2     // Bouton 1
#define BUTTON2_PIN 3     // Bouton 2
#define STOP_BUTTON 6     // Bouton d'arrêt de l'alarme
#define INA_PIN 4         // Contrôle moteur (Porte)
#define INB_PIN 5         // Contrôle moteur (Porte)

int lightValue = 0;
bool alarmActive = false;
int alarmMode = 0; // 0: Mode simple, 1: Mode alarme
int failedAttempts = 0;
unsigned long lastPresenceCheck = 0; // Pour vérifier la présence toutes les 5s

// Définition des utilisateurs et leurs codes
struct User {
  String name;
  int button1Count;
  int button2Count;
};

User users[] = {
  {"Aziz", 4, 0},
  {"Mohammed", 2, 2},
  {"Mehdi", 1, 3}
};

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);
  pinMode(BUTTON1_PIN, INPUT_PULLUP);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);
  pinMode(STOP_BUTTON, INPUT_PULLUP);
  pinMode(INA_PIN, OUTPUT);
  pinMode(INB_PIN, OUTPUT);
  Serial.begin(9600);  // Begin serial communication at 9600 baud
}

void loop() {
  lightValue = analogRead(SENSOR_PIN);

  // Détection de présence toutes les 5s
  if (millis() - lastPresenceCheck >= 5000) {
    lastPresenceCheck = millis();
    if (lightValue >= 380) { // Si absence détectée
      Serial.println("🚨 Personne détectée, alarme désactivée !");
      stopAlarm();
    }
  }

  if (lightValue < 380 && !alarmActive) {
    activateAlarm();
  }

  if (alarmActive) {
    if (digitalRead(STOP_BUTTON) == LOW) {
      stopAlarm();
    } else if (alarmMode == 0) {
      checkUserCode();
    } else {
      alertMode();
    }
  }

  sendStatus();
  delay(100);
}

void activateAlarm() {
  alarmActive = true;
  alarmMode = 0;
  failedAttempts = 0;
  digitalWrite(LED_PIN, HIGH);
  Serial.println("🚨 Alarme activée (Mode simple) !");
}

void activateAlertMode() {
  alarmMode = 1;
  Serial.println("⚠ ALERT! ALERT! Mode Alarme activé !");
}

void stopAlarm() {
  alarmActive = false;
  failedAttempts = 0;
  digitalWrite(LED_PIN, LOW);
  Serial.println("🛑 Alarme arrêtée !");
}

void openDoor(String userName) {
  Serial.print("🚪 Accès autorisé : ");
  Serial.println(userName);

  digitalWrite(INA_PIN, HIGH);
  digitalWrite(INB_PIN, LOW);
  delay(5000);
  digitalWrite(INA_PIN, LOW);
  digitalWrite(INB_PIN, LOW);

  stopAlarm();
}

void checkUserCode() {
  int button1Count = 0;
  int button2Count = 0;
  unsigned long startTime = millis();
  bool userEnteredCode = false;

  while (millis() - startTime < 5000) {
    if (digitalRead(BUTTON1_PIN) == LOW) {
      button1Count++;
      Serial.println("Bouton 1 pressé");
      delay(300);
    }
    if (digitalRead(BUTTON2_PIN) == LOW) {
      button2Count++;
      Serial.println("Bouton 2 pressé");
      delay(300);
    }

    if (button1Count + button2Count == 4) {
      userEnteredCode = true;
      break;
    }
  }

  if (userEnteredCode) {
    String user = verifyCode(button1Count, button2Count);
    if (user != "") {
      openDoor(user);
    } else {
      Serial.println("❌ Code incorrect !");
      failedAttempts++;
      if (failedAttempts >= 3) {
        activateAlertMode(); // Activating alert mode and it stays on forever
      } else {
        Serial.print("⏳ Tentatives restantes: ");
        Serial.print(failedAttempts);
        Serial.println("/3");
        Serial.println("⏳ Attente de 2 secondes avant nouvelle tentative...");
        delay(2000);
      }
    }
  } else {
    Serial.println("Aucune saisie de code détectée.");
  }
}

String verifyCode(int button1Count, int button2Count) {
  for (User user : users) {
    if (user.button1Count == button1Count && user.button2Count == button2Count) {
      return user.name;
    }
  }
  return "";
}

void alertMode() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
  Serial.println("⚠ ALERT! ALERT! 🚨");
}

void sendStatus() {
  String status = "{";
  status += "\"alarme\": " + String(alarmActive ? "true" : "false") + ", ";
  status += "\"mode\": \"" + String(alarmMode == 0 ? "simple" : "alert") + "\", ";
  status += "\"porte\": \"" + String(digitalRead(INA_PIN) == HIGH ? "open" : "closed") + "\"";
  status += "}";

  Serial.println(status);  // Send status as JSON string to the serial port
}
