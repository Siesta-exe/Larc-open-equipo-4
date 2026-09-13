import cv2 as cv
import numpy as np
import requests

IP_ESP32 = "192.168.8.125"
url = f"http://{IP_ESP32}:81/stream"
# Encender la linterna/flash de la ESP32-CAM
requests.get(f"http://{IP_ESP32}/control?var=led_intensity&val=255")
cap = cv.VideoCapture(url)

if not cap.isOpened():
    print("No se pudo conectar con la ESP32-CAM")
    exit()

print("Conectado a la ESP32-CAM")
print("Presiona ESC para salir")


# --- AZUL ---
azul_lower = np.array([106, 38,  40])
azul_upper = np.array([125, 176, 190])

# --- AMARILLO ---
amarillo_lower = np.array([10, 35, 134])
amarillo_upper = np.array([33, 108, 242])

# --- ROJO / NARANJA (dos rangos porque el rojo envuelve 0/180) ---
rojo_lower_1 = np.array([167,   90,  55])
rojo_upper_1 = np.array([179,  194, 200])
                             #175   #163
naranja_lower = np.array([171, 96,  70])
naranja_upper = np.array([179, 255, 255])

AREA_MINIMA = 150           # descarta ruido muy pequeño (no depende de la distancia real)
CIRCULARIDAD_MINIMA = 0.65  # 1.0 = círculo perfecto; baja esto si pierdes pelotas lejanas
KERNEL = np.ones((5, 5), np.uint8)

def detectar_en_mascara(mascara, nombre_color):
    detecciones = []

    # Limpieza morfológica: quita ruido pequeño y rellena huecos
    mascara = cv.morphologyEx(mascara, cv.MORPH_OPEN, KERNEL)
    mascara = cv.morphologyEx(mascara, cv.MORPH_CLOSE, KERNEL)

    contornos, _ = cv.findContours(
        mascara,
        cv.RETR_EXTERNAL,
        cv.CHAIN_APPROX_SIMPLE
    )

    for contorno in contornos:
        area = cv.contourArea(contorno)
        if area < AREA_MINIMA:
            continue

        perimetro = cv.arcLength(contorno, True)
        if perimetro == 0:
            continue

        circularidad = 4 * np.pi * area / (perimetro ** 2)
        if circularidad < CIRCULARIDAD_MINIMA:
            continue

        (x, y), radio = cv.minEnclosingCircle(contorno)

        detecciones.append({
            "color": nombre_color,
            "centro": (int(x), int(y)),
            "radio": int(radio),
            "area": area,
            "circularidad": circularidad
        })

    return detecciones

nombre_ventana = 'Deteccion de objetos'
cv.namedWindow(nombre_ventana, cv.WINDOW_NORMAL)

while cv.waitKey(1) != 27:

    has_frame, frame = cap.read()
    if not has_frame:
        print("No se pudo recibir imagen")
        break

    hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)

    # Reduce un poco el ruido de la imagen antes de umbralizar
    hsv = cv.GaussianBlur(hsv, (5, 5), 0)

    # MÁSCARAS POR COLOR
    mascara_azul = cv.inRange(hsv, azul_lower, azul_upper)
    mascara_amarillo = cv.inRange(hsv, amarillo_lower, amarillo_upper)
    #mascara_rojo = cv.inRange(hsv, rojo_lower_1, rojo_upper_1)
    mascara_rojo = cv.bitwise_or(
        cv.inRange(hsv, rojo_lower_1, rojo_upper_1),
        cv.inRange(hsv, naranja_lower, naranja_upper))
    #mascara_naranja = cv.inRange(hsv, naranja_lower, naranja_upper)

    # Naranja cuenta como rojo
    #mascara_rojo = cv.bitwise_or(mascara_rojo, mascara_naranja)

    detecciones = []
    detecciones += detectar_en_mascara(mascara_azul, "Pelota azul")
    detecciones += detectar_en_mascara(mascara_amarillo, "Pelota amarilla")
    detecciones += detectar_en_mascara(mascara_rojo, "Pelota roja")

    colores_dibujo = {
        "Pelota azul": (255, 0, 0),
        "Pelota amarilla": (0, 255, 255),
        "Pelota roja": (0, 0, 255),
    }

    for d in detecciones:
        x, y = d["centro"]
        r = d["radio"]
        color_bgr = colores_dibujo[d["color"]]

        cv.circle(frame, (x, y), r, color_bgr, 3)
        cv.circle(frame, (x, y), 4, (255, 255, 255), -1)

        cv.putText(
            frame,
            d["color"],
            (x - r, y - r - 10),
            cv.FONT_HERSHEY_SIMPLEX,
            0.6,
            color_bgr,
            2
        )

    cv.putText(
        frame,
        f"Pelotas detectadas: {len(detecciones)}",
        (10, 25),
        cv.FONT_HERSHEY_SIMPLEX,
        0.7,
        (255, 255, 255),
        2
    )

    cv.imshow(nombre_ventana, frame)

cap.release()
cv.destroyAllWindows()