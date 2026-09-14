// Definición de los estados de navegación
enum EstadoNavegacion {
  SEGUIDOR_LINEA,
  GIRO_DESVIO,
  SEGUIDOR_PARED,
  SOBREPASO,
  BUSQUEDA_LINEA
};

// El robot inicia su ciclo siguiendo la línea negra
EstadoNavegacion estadoActual = SEGUIDOR_LINEA;

// Definición de pines (Ajustar según cableado de la ESP32)
const int trigFrente = 5;
const int echoFrente = 18;
const int trigLateral = 19;
const int echoLateral = 21;

void setup() {
  Serial.begin(115200);
  pinMode(trigFrente, OUTPUT);
  pinMode(echoFrente, INPUT);
  pinMode(trigLateral, OUTPUT);
  pinMode(echoLateral, INPUT);
  
  // TODO: Declarar pinMode para motores e infrarrojos
}

void loop() {
  switch (estadoActual) {
    
    case SEGUIDOR_LINEA:
      seguirLineaIR(); // Lógica de tu compañera
      
      // Tu validación: Si la piscina está a menos de 20cm
      if (leerDistancia(trigFrente, echoFrente) < 20) {
        pararMotores();
        estadoActual = GIRO_DESVIO; 
      }
      break;

    case GIRO_DESVIO:
      girarDerecha(); // Función de la pestaña de Motores
      // Aquí puedes usar millis() en lugar de delay para no bloquear la ESP32
      pararMotores();
      estadoActual = SEGUIDOR_PARED;
      break;

    case SEGUIDOR_PARED:
      // Tu control: Mantener el robot a ~15 cm de la piscina
      int distLateral = leerDistancia(trigLateral, echoLateral);
      avanzarControlado(distLateral); 
      
      if (distLateral > 30) { // Si lee al vacío, terminó la piscina
        estadoActual = SOBREPASO;
      }
      break;

    case SOBREPASO:
      avanzar();
      delay(300); // Avance extra para limpiar la esquina de la piscina
      girarIzquierda();
      estadoActual = BUSQUEDA_LINEA;
      break;

    case BUSQUEDA_LINEA:
      avanzar();
      // Tu compañera detecta el retorno a la cinta negra
      if (sensorIR_detectaLinea()) { 
        estadoActual = SEGUIDOR_LINEA; 
      }
      break;
  }
}