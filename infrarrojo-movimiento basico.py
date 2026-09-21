from machine import Pin, PWM
import time

# ==========================================
# Pines de la Regleta IR de 8 Canales
# ==========================================

pines_ir_ids = [13, 21, 14, 27, 26, 25, 33, 32] 
pines_ir = [Pin(pin_id, Pin.IN) for pin_id in pines_ir_ids]

# ==========================================
#  Pines del Driver de Motores 
# ==========================================

pin_IN1 = Pin(4, Pin.OUT)
pin_IN2 = Pin(19, Pin.OUT)
pwm_ENA = PWM(Pin(15), freq=1000) # PWM Motor Izquierdo

pin_IN3 = Pin(18, Pin.OUT)
pin_IN4 = Pin(16, Pin.OUT)
pwm_ENB = PWM(Pin(17), freq=1000) # PWM Motor Derecho


VELOCIDAD_NORMAL = 700
VELOCIDAD_LENTA  = 480
VELOCIDAD_GIRO   = 600

# ==========================================
# Inicialización de Hardware
# ==========================================

def setup_infrarrojos_y_motores():
    """Inicializa los motores en estado detenido."""
    parar_motores()

def leer_regleta_ir():

    return [bool(pin.value()) for pin in pines_ir]

# ==========================================
# Funciones de Control de Movimiento y Navegacion
# ==========================================

def seguir_linea_ir():

    ir_val = leer_regleta_ir()

    if ir_val[3] and not ir_val[4]:
        avanzar()
    elif ir_val[4] or ir_val[5]:
        corregir_derecha()
    elif not ir_val[3] and not ir_val[2]:
        corregir_izquierda()
    else:
        avanzar_lento()

def sensor_ir_detecta_linea():
    ir_val = leer_regleta_ir()
    return any(ir_val)

def avanzar_controlado_pared(dist_actual, dist_objetivo=15):

    error = dist_actual - dist_objetivo
    kp = 25  # Ganancia proporcional
    
    vel_izq = max(300, min(900, VELOCIDAD_NORMAL - int(error * kp)))
    vel_der = max(300, min(900, VELOCIDAD_NORMAL + int(error * kp)))
    
    pin_IN1.value(1); pin_IN2.value(0)
    pin_IN3.value(1); pin_IN4.value(0)
    pwm_ENA.duty(vel_izq)
    pwm_ENB.duty(vel_der)

# ==========================================
# Comandos Directos a Motores
# ==========================================

def avanzar():
    pin_IN1.value(1); pin_IN2.value(0)
    pin_IN3.value(1); pin_IN4.value(0)
    pwm_ENA.duty(VELOCIDAD_NORMAL)
    pwm_ENB.duty(VELOCIDAD_NORMAL)

def avanzar_lento():
    pin_IN1.value(1); pin_IN2.value(0)
    pin_IN3.value(1); pin_IN4.value(0)
    pwm_ENA.duty(VELOCIDAD_LENTA)
    pwm_ENB.duty(VELOCIDAD_LENTA)

def retroceder():
    pin_IN1.value(0); pin_IN2.value(1)
    pin_IN3.value(0); pin_IN4.value(1)
    pwm_ENA.duty(VELOCIDAD_NORMAL)
    pwm_ENB.duty(VELOCIDAD_NORMAL)

def parar_motores():
    pin_IN1.value(0); pin_IN2.value(0)
    pin_IN3.value(0); pin_IN4.value(0)
    pwm_ENA.duty(0)
    pwm_ENB.duty(0)

def girar_derecha():
    pin_IN1.value(1); pin_IN2.value(0)
    pin_IN3.value(0); pin_IN4.value(1)
    pwm_ENA.duty(VELOCIDAD_GIRO)
    pwm_ENB.duty(VELOCIDAD_GIRO)

def girar_izquierda():
    pin_IN1.value(0); pin_IN2.value(1)
    pin_IN3.value(1); pin_IN4.value(0)
    pwm_ENA.duty(VELOCIDAD_GIRO)
    pwm_ENB.duty(VELOCIDAD_GIRO)

def corregir_derecha():
    pin_IN1.value(1); pin_IN2.value(0)
    pin_IN3.value(1); pin_IN4.value(0)
    pwm_ENA.duty(VELOCIDAD_NORMAL)
    pwm_ENB.duty(VELOCIDAD_LENTA)

def corregir_izquierda():
    pin_IN1.value(1); pin_IN2.value(0)
    pin_IN3.value(1); pin_IN4.value(0)
    pwm_ENA.duty(VELOCIDAD_LENTA)
    pwm_ENB.duty(VELOCIDAD_NORMAL)