#define LED_PIN 13     // Pin de la LED
#define SENSOR_PIN A0  // Pin du capteur de lumière

void setup() {
  pinMode(LED_PIN, OUTPUT);  // Définit la LED en sortie
  pinMode(SENSOR_PIN, INPUT); // Capteur en entrée
  Serial.begin(9600);  // Initialise la communication série
}

void loop() {
  int lightValue = analogRead(SENSOR_PIN); // Lire la valeur du capteur

  // Afficher la valeur de luminosité sur la console série
  Serial.print("Luminosité : ");
  Serial.println(lightValue);

  if (lightValue > 500) {  // Si la valeur est supérieure à un seuil (ajuste si nécessaire)
    digitalWrite(LED_PIN, HIGH); // Allumer la LED
  } else {
    digitalWrite(LED_PIN, LOW); // Éteindre la LED
  }

  delay(100); // Petite pause pour éviter trop de lectures
}
