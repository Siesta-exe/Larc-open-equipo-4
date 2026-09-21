// ==========================================
// Pestaña Principal: Robot_LARC.ino
// ==========================================

enum EstadoNavegacion {
  SEGUIDOR_LINEA,
  GIRO_DESVIO,
  SEGUIDOR_PARED,
  LIBERAR_ESQUINA,
  REINCORPORACION_LINEA,
  BUSQUEDA_LINEA
};

EstadoNavegacion estadoActual = SEGUIDOR_LINEA;

// Pines Ultrasonidos (ESP32)
const int trigFrenteBajo = 2;   // Cambiado a GPIO 4 (Evita conflicto de Boot)
const int echoFrenteBajo = 3;

const int trigFrenteAlto = 4;
const int echoFrenteAlto = 5;

const int trigLateral = 6;
const int echoLateral = 7;

// Umbrales de distancia (cm)
const int DIST_DETECCION_PISCINA = 18; 
const int DIST_PARED_OBJETIVO = 15;    
const int DIST_PARED_PERDIDA = 35;     

unsigned long tiempoEstado = 0; 

void setup() {
  Serial.begin(115200);
  delay(500); 
  
  Serial.println("=================================");
  Serial.println("ESP32 INICIADO CORRECTAMENTE");
  Serial.println("=================================");

  // Inicialización centralizada de los pines
  setupUltrasonidos();
}

void loop() {
  
  int distBajo = 0;
  int distLat = 0;

  switch (estadoActual) {
    
    case SEGUIDOR_LINEA:
      seguirLineaIR(); // Control de borde externo con TCRT5000
      
      // Muestreo del sensor de detección de piscina (Frente-Bajo)
      distBajo = leerDistancia(trigFrenteBajo, echoFrenteBajo);
      if (distBajo < DIST_DETECCION_PISCINA) {
        pararMotores();
        tiempoEstado = millis();
        estadoActual = GIRO_DESVIO; 
      }
      break;

    case GIRO_DESVIO:
      // Giro de 90° a la derecha para alinearse paralelo a la piscina
      girarDerecha(); 
      if (millis() - tiempoEstado > 650) { // Ajustar ms según calibración PWM
        pararMotores();
        estadoActual = SEGUIDOR_PARED;
      }
      break;

    case SEGUIDOR_PARED:
      distLat = leerDistancia(trigLateral, echoLateral);
      
      if (distLat < DIST_PARED_PERDIDA) {
        // P-Controller simple o corrección Bang-Bang para mantener ~15cm
        avanzarControladoPared(distLat, DIST_PARED_OBJETIVO); 
      } else {
        // La pared terminó (se superó el largo de 720 mm de la piscina)
        pararMotores();
        tiempoEstado = millis();
        estadoActual = LIBERAR_ESQUINA;
      }
      break;

    case LIBERAR_ESQUINA:
      // Avanzar un tramo recto adicional para librar el ancho de la piscina (200 mm)
      avanzar();
      if (millis() - tiempoEstado > 500) { 
        pararMotores();
        tiempoEstado = millis();
        estadoActual = REINCORPORACION_LINEA;
      }
      break;

    case REINCORPORACION_LINEA:
      // Giro a la izquierda de 90° para apuntar de vuelta hacia la línea principal
      girarIzquierda();
      if (millis() - tiempoEstado > 650) {
        pararMotores();
        tiempoEstado = millis();
        estadoActual = BUSQUEDA_LINEA;
      }
      break;

    case BUSQUEDA_LINEA:
      avanzarLento();
      
      // Detección de retorno a la línea negra con los TCRT5000
      if (sensorIR_detectaLinea()) { 
        pararMotores();
        estadoActual = SEGUIDOR_LINEA; 
      }
      
      // Watchdog de seguridad: Si avanza por más de 3.5s sin hallar línea, frena
      if (millis() - tiempoEstado > 3500) {
        pararMotores();
        // Manejar error o re-orientar
      }
      break;
  }
}