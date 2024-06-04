/*
* PROYECTO: ASCENSOR 
*/

#include <Keypad.h>  // https://github.com/Nullkraft/Keypad (3.2.1)


// Distribucion de las entradas digitales; usamos "byte" en vez de "int" por dependecia de la libreria.
#define pin_giro_pos_mot 9   // Pin sentido 1 motor
#define pin_giro_neg_mot 10  // Pin sentido 2 motor
#define pin_vel_mot 11       // Pin velocidad L298N

#define velocidad_motor 50  // Velocidad motor (%)

// Keypad teclado
#define filas_keypad 4
#define columnas_keypad 3
byte pines_fila[filas_keypad] = { 8, 7, 6, 5};
byte pines_columna[columnas_keypad] = {4, 3, 2};
char matriz_teclas[filas_keypad][columnas_keypad] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

// Finales de carrera (pines A0 - A3)
#define pin_piso0 A0
#define pin_piso1 A1
#define pin_piso2 A2
#define pin_piso3 A3

// Creacion de los objetos y variables del programa
Keypad almohadilla = Keypad(makeKeymap(matriz_teclas), pines_fila, pines_columna, filas_keypad, columnas_keypad);
int piso0, piso1, piso2, piso3;
int piso_actual = -1;
int piso_escogido = -1;
int velocidad_real = map(velocidad_motor, 0, 100, 0, 255);  // Regla de 3 entre 0-100 y 0-255 sengun el % de velocidad

void setup() {
  Serial.begin(9600);

  inicializar();
  Serial.println("Setup finalizado");
  delay(2000);
}


void loop() {
  // Si la parada actual es la misma que la de destino para y pide numbero
  if (piso_actual == piso_escogido) {
    parar_ascensor();
    char tecla_pulsada = almohadilla.getKey();
    if (tecla_pulsada) {
      Serial.print("Tecla pulsada: ");
      Serial.println(tecla_pulsada);
      // procesar_movimiento(tecla_pulsada - '0');
      piso_escogido = tecla_pulsada - '0';
    }
  }

  while (piso_actual != piso_escogido){
    mover_ascensor(piso_actual < piso_escogido);
    int temp_actual = comprobar_piso_activado();
  if (temp_actual != -1){
    piso_actual = temp_actual;
  }
  }
}

  // se ha pulsado una tecla. comprobar si es mayor o menor el piso actual

int comprobar_piso_activado() {
  if(analogRead(A0) * (5.0 / 1023.0) > 4.8) {
    Serial.println("pin_piso0");
    return 0;
  }
  if (analogRead(A1) * (5.0 / 1023.0) > 4.8) {
    Serial.println("pin_piso1");
    return 1;
  }
  if (analogRead(A2) * (5.0 / 1023.0) > 4.8) {
    Serial.println("pin_piso2");
    return 2;
  }
    
  if (analogRead(A3) * (5.0 / 1023.0) > 4.8) {
    Serial.println("pin_piso3");
    return 3;
    }

  return -1;
}


void procesar_movimiento(int tecla_pulsada) {
  if (tecla_pulsada == piso_actual) {
    parar_ascensor();
    return;
  }
  piso_escogido = tecla_pulsada;

  if (piso_actual < tecla_pulsada) {
    mover_ascensor(true);
  } else {
    mover_ascensor(false);
  }

}

/**
Mueve el ascensor a la planta baja.
**/
void inicializar() {
  piso_actual = -1;
  while (piso_actual != 0) {
    mover_ascensor(false);
    piso_actual = comprobar_piso_activado();
    Serial.print("piso actual INICIALIZAR:");
    Serial.println(piso_actual);
  }
  parar_ascensor();
  piso_escogido = 0;
  piso_actual = 0;
}

/*
* False es bajar
*/
void mover_ascensor(bool subir) {
  analogWrite(pin_vel_mot, velocidad_real);
  Serial.print("Ascensor: ");
  digitalWrite(pin_giro_neg_mot, !subir);
  digitalWrite(pin_giro_pos_mot, subir);
  if (subir) {
    Serial.print("Subiendo");
  } else {
    Serial.print("Bajando");
  }
  Serial.print(" a planta ");
  Serial.println(piso_escogido);
}

void parar_ascensor() {
  analogWrite(pin_vel_mot, 0);
  digitalWrite(pin_giro_neg_mot, 0);
  digitalWrite(pin_giro_pos_mot, 0);
  Serial.println("Ascensor parado");
}
