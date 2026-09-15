void setup() {
  // Comunicación serial a 9600 baudios
  Serial.begin(9600);

  // LED conectado al GPIO 13
  pinMode(13, OUTPUT);
}

void loop() {
  // Leemos el promedio de 20 mediciones del Sharp
  int ADC_SHARP = ADC0_promedio(20);

  if (ADC_SHARP > 150) {
    digitalWrite(13, HIGH);
    Serial.print("Objeto Detectado: ");
  }
  else {
    digitalWrite(13, LOW);
    Serial.print("Objeto ausente: ");
  }

  Serial.println(ADC_SHARP);

  delay(10);
}

// Función para calcular el promedio de las lecturas
int ADC0_promedio(int n) {
  long suma = 0;

  for (int i = 0; i < n; i++) {
    suma = suma + analogRead(34);  // GPIO 34 de la ESP32
  }

  return (suma / n);
}
