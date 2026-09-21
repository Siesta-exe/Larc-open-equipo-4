// ============================================================================
// MÓDULO DE ULTRASONIDOS (Ultrasonidos.ino)
// ============================================================================

const unsigned long TIMEOUT_US = 30000; // Timeout de 15 ms (~250 cm max)

void setupUltrasonidos() {
  pinMode(trigFrenteBajo, OUTPUT);
  pinMode(echoFrenteBajo, INPUT);
  
  pinMode(trigFrenteAlto, OUTPUT);
  pinMode(echoFrenteAlto, INPUT);
  
  pinMode(trigLateral, OUTPUT);
  pinMode(echoLateral, INPUT);

  digitalWrite(trigFrenteBajo, LOW);
  digitalWrite(trigFrenteAlto, LOW);
  digitalWrite(trigLateral, LOW);
}

int medirDistanciaBase(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(4);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duracion = pulseIn(echoPin, HIGH, TIMEOUT_US);

  if (duracion == 0) return 999; 

  return (int)(duracion * 0.0343 / 2.0); 
}

// Filtro de Mediana corregido con pausas de 20 ms para disipar ecos
int leerDistanciaFiltrada(int trigPin, int echoPin) {
  int m1 = medirDistanciaBase(trigPin, echoPin);
  delay(20); 
  int m2 = medirDistanciaBase(trigPin, echoPin);
  delay(20);
  int m3 = medirDistanciaBase(trigPin, echoPin);

  // Retorna la mediana de las 3 muestras
  if ((m1 <= m2 && m2 <= m3) || (m3 <= m2 && m2 <= m1)) return m2;
  if ((m2 <= m1 && m1 <= m3) || (m3 <= m1 && m1 <= m2)) return m1;
  return m3;
}

bool detectaPiscinaFrente() {
  int dist = leerDistanciaFiltrada(trigFrenteBajo, echoFrenteBajo);
  return (dist < 18); 
}

bool detectaObstaculoAlto() {
  int dist = leerDistanciaFiltrada(trigFrenteAlto, echoFrenteAlto);
  return (dist < 20);
}

int obtenerDistanciaLateral() {
  return leerDistanciaFiltrada(trigLateral, echoLateral);
}

// Función de prueba optimizada sin doble muestreo
void probarUltrasonidos() {
  //Para probar ultrasonidos individualmente cambiar su definición
  int distBajo = leerDistanciaFiltrada(trigFrenteBajo, echoFrenteBajo);
  bool piscina = (distBajo < 18); // Usa la misma lectura filtrada en lugar de re-medir
  
  Serial.print("F.BAJO: ");
  if (distBajo == 999) {
    Serial.print("ERR");
  } else {
    Serial.print(distBajo);
    Serial.print(" cm");
  }
  
  Serial.print(" | Piscina: ");
  Serial.println(piscina ? "DETECTADA" : "LIBRE");

  delay(100); 
}