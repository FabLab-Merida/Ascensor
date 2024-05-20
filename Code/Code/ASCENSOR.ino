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
byte pines_fila[filas_keypad] = { 2, 3, 4, 5 };
byte pines_columna[columnas_keypad] = { 6, 7, 8 };
char matriz_teclas[filas_keypad][columnas_keypad] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

// Finales de carrera (pines A0 - A3)
#define pin_piso0 14
#define pin_piso1 15
#define pin_piso2 16
#define pin_piso3 17




// Creacion de los objetos y variables del programa
Keypad almohadilla = Keypad(makeKeymap(matriz_teclas), pines_fila, pines_columna, filas_keypad, columnas_keypad);
int piso0, piso1, piso2, piso3;

// Funciones del programa
void ascensor(int piso_destino);
void test_motor(boolean sentido);

void setup() {
  Serial.begin(9600);
}

void loop() {
  char tecla_pulsada = almohadilla.getKey();
  if (tecla_pulsada) {
    Serial.print("Tecla pulsada: ");
    Serial.println(tecla_pulsada);
    ascensor(tecla_pulsada - '0');
  }
}

void ascensor(int piso_destino) {
  int velocidad_real = map(velocidad_motor, 0, 100, 0, 255);  // Regla de 3 entre 0-100 y 0-255 sengun el % de velocidad
  int piso_act;
  bool tarea_hecha = false;
  bool todo_ok = false;
  piso0 = digitalRead(pin_piso0);
  piso1 = digitalRead(pin_piso1);
  piso2 = digitalRead(pin_piso2);
  piso3 = digitalRead(pin_piso3);

  if (piso_destino <= 3 && piso_destino >= 0) {
    if ((piso0 == 1 && ((piso1 == piso2) == piso3) == 0) || (piso1 == 1 && ((piso0 == piso2) == piso3) == 0) || (piso2 == 1 && ((piso1 == piso0) == piso0) == 0) || (piso3 == 1 && ((piso1 == piso2) == piso3) == 0)) {
      if (piso0 == 1) {
        piso_act = 0;
        todo_ok = true;
      } else if (piso1 == 1) {
        piso_act = 1;
        todo_ok = true;
      } else if (piso2 == 1) {
        piso_act = 2;
        todo_ok = true;
      } else if (piso3 == 1) {
        piso_act = 3;
        todo_ok = true;
      }
    } else {
      Serial.print("ERR. EN FINALES DE CARRERA: ");
      Serial.print(piso0);
      Serial.print(" ");
      Serial.print(piso1);
      Serial.print(" ");
      Serial.print(piso2);
      Serial.print(" ");
      Serial.println(piso3);
    }
  } else {
    Serial.println("ERR. NUMERO FUERA DE RANGO ");
  }

  //SUBIDA
  if (piso_destino > piso_act && tarea_hecha == false && todo_ok == true) {
    Serial.print("ESTADO FINALES CARRERA: ");
    Serial.print(piso0);
    Serial.print(" ");
    Serial.print(piso1);
    Serial.print(" ");
    Serial.print(piso2);
    Serial.print(" ");
    Serial.println(piso3);

    Serial.println("SUBIENDO..");
    while (piso_act != piso_destino) {
      piso0 = digitalRead(pin_piso0);
      piso1 = digitalRead(pin_piso1);
      piso2 = digitalRead(pin_piso2);
      piso3 = digitalRead(pin_piso3);
      if (piso0 == 1) {
        piso_act = 0;
      } else if (piso1 == 1) {
        piso_act = 1;
      } else if (piso2 == 1) {
        piso_act = 2;
      } else if (piso3 == 1) {
        piso_act = 3;
      }
      analogWrite(pin_vel_mot, velocidad_real);
      digitalWrite(pin_giro_neg_mot, 0);
      digitalWrite(pin_giro_pos_mot, 1);
    }
    tarea_hecha = true;
  }

  //BAJADA
  if (piso_destino < piso_act && tarea_hecha == false && todo_ok == true) {
    Serial.print("ESTADO FINALES CARRERA: ");
    Serial.print(piso0);
    Serial.print(" ");
    Serial.print(piso1);
    Serial.print(" ");
    Serial.print(piso2);
    Serial.print(" ");
    Serial.println(piso3);

    Serial.println("BAJANDO..");
    while (piso_act != piso_destino) {
      piso0 = digitalRead(pin_piso0);
      piso1 = digitalRead(pin_piso1);
      piso2 = digitalRead(pin_piso2);
      piso3 = digitalRead(pin_piso3);
      if (piso0 == 1) {
        piso_act = 0;
      } else if (piso1 == 1) {
        piso_act = 1;
      } else if (piso2 == 1) {
        piso_act = 2;
      } else if (piso3 == 1) {
        piso_act = 3;
      }
      analogWrite(pin_vel_mot, velocidad_real);
      digitalWrite(pin_giro_neg_mot, 1);
      digitalWrite(pin_giro_pos_mot, 0);
    }
    tarea_hecha = true;
  }

  // MISMO PISO
  if (piso_destino == piso_act && tarea_hecha == false && todo_ok == true) {
    Serial.print("ESTADO FINALES CARRERA: ");
    Serial.print(piso0);
    Serial.print(" ");
    Serial.print(piso1);
    Serial.print(" ");
    Serial.print(piso2);
    Serial.print(" ");
    Serial.println(piso3);

    Serial.println("ERR. YA ESTAS EN ESE PISO");
    tarea_hecha = true;
  }
  analogWrite(pin_vel_mot, 0);
  digitalWrite(pin_giro_neg_mot, 0);
  digitalWrite(pin_giro_pos_mot, 0);
  Serial.println("FINAL OPERACION");
}





void test_motor(boolean sentido) {
  int velocidad_real = map(velocidad_motor, 0, 100, 0, 255);  // Regla de 3 entre 0-100 y 0-255 sengun el % de velocidad
  Serial.print(velocidad_real);
  if (sentido) {
    analogWrite(pin_vel_mot, velocidad_real);
    digitalWrite(pin_giro_neg_mot, 1);
    digitalWrite(pin_giro_pos_mot, 0);
    delay(2500);
  } else {
    analogWrite(pin_vel_mot, velocidad_real);
    digitalWrite(pin_giro_neg_mot, 0);
    digitalWrite(pin_giro_pos_mot, 1);
    delay(2500);
  }
  analogWrite(pin_vel_mot, 0);
  digitalWrite(pin_giro_neg_mot, 0);
  digitalWrite(pin_giro_pos_mot, 0);
}
