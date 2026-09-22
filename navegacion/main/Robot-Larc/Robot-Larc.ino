// ==========================================
// Pestaña Principal: Robot_LARC.ino (Arduino Uno)
// ==========================================

enum EstadoNavegacion {
  SEGUIDOR_LINEA,
  GIRO_DESVIO,
  SEGUIDOR_PARED,
  LIBERAR_ESQUINA,
  REINCORPORACION_LINEA,
  BUSQUEDA_LINEA
};

EstadoNavegacion estadoActual = SEGUIDOR_LINEA; //[cite: 3]

// Pines Ultrasonidos para Arduino Uno (Pines 2 al 7)[cite: 3]
const int trigFrenteBajo = 2; //[cite: 3]
const int echoFrenteBajo = 3; //[cite: 3]

const int trigFrenteAlto = 4; //[cite: 3]
const int echoFrenteAlto = 5; //[cite: 3]

const int trigLateral = 6;    //[cite: 3]
const int echoLateral = 7;    //[cite: 3]

// Umbrales de distancia (cm)[cite: 3]
const int DIST_DETECCION_PISCINA = 18; //[cite: 3]
const int DIST_PARED_OBJETIVO = 15;    //[cite: 3]
const int DIST_PARED_PERDIDA = 35;     //[cite: 3]

unsigned long tiempoEstado = 0; //[cite: 3]

void setup() {
  Serial.begin(115200); //[cite: 3]
  delay(500); //[cite: 3]
  
  Serial.println("=================================");
  Serial.println("ARDUINO UNO INICIADO CORRECTAMENTE");
  Serial.println("=================================");

  // Inicialización de pines mediante Ultrasonidos.ino
  setupUltrasonidos(); //[cite: 3]
}

void loop() {
  int distBajo = 0; //[cite: 3]
  int distLat = 0;  //[cite: 3]

  switch (estadoActual) { //[cite: 3]
    
    case SEGUIDOR_LINEA: //[cite: 3]
      // seguirLineaIR(); // TODO: Implementar control con TCRT5000[cite: 3]
      
      // Muestreo corregido del sensor Frente-Bajo usando la lectura filtrada[cite: 3, 4]
      distBajo = leerDistanciaFiltrada(trigFrenteBajo, echoFrenteBajo); //[cite: 4]
      if (distBajo < DIST_DETECCION_PISCINA) { //[cite: 3]
        // pararMotores();[cite: 3]
        tiempoEstado = millis(); //[cite: 3]
        estadoActual = GIRO_DESVIO; //[cite: 3]
      }
      break;

    case GIRO_DESVIO: //[cite: 3]
      // girarDerecha(); //[cite: 3]
      if (millis() - tiempoEstado > 650) { //[cite: 3]
        // pararMotores();[cite: 3]
        estadoActual = SEGUIDOR_PARED; //[cite: 3]
      }
      break;

    case SEGUIDOR_PARED: //[cite: 3]
      // Muestreo corregido con la función de alto nivel del sensor lateral[cite: 3, 4]
      distLat = obtenerDistanciaLateral(); //[cite: 4]
      
      if (distLat < DIST_PARED_PERDIDA) { //[cite: 3]
        // avanzarControladoPared(distLat, DIST_PARED_OBJETIVO);[cite: 3]
      } else {
        // pararMotores();[cite: 3]
        tiempoEstado = millis(); //[cite: 3]
        estadoActual = LIBERAR_ESQUINA; //[cite: 3]
      }
      break;

    case LIBERAR_ESQUINA: //[cite: 3]
      // avanzar();[cite: 3]
      if (millis() - tiempoEstado > 500) { //[cite: 3]
        // pararMotores();[cite: 3]
        tiempoEstado = millis(); //[cite: 3]
        estadoActual = REINCORPORACION_LINEA; //[cite: 3]
      }
      break;

    case REINCORPORACION_LINEA: //[cite: 3]
      // girarIzquierda();[cite: 3]
      if (millis() - tiempoEstado > 650) { //[cite: 3]
        // pararMotores();[cite: 3]
        tiempoEstado = millis(); //[cite: 3]
        estadoActual = BUSQUEDA_LINEA; //[cite: 3]
      }
      break;

    case BUSQUEDA_LINEA: //[cite: 3]
      // avanzarLento();[cite: 3]
      
      /* TODO: Implementar sensores IR
      if (sensorIR_detectaLinea()) {[cite: 3]
        pararMotores();[cite: 3]
        estadoActual = SEGUIDOR_LINEA;[cite: 3]
      }
      */
      
      if (millis() - tiempoEstado > 3500) { //[cite: 3]
        // pararMotores();[cite: 3]
      }
      break;
  }
}