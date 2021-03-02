#include <LiquidCrystal.h>
#include <Keypad.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>

//crear un objeto LiquidCrystal (rs,enable,d4,d5,d6,d7)
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

//Variables teclado
const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 3; //three columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

//Variable temporal que almacena las cadenas recibidas
String cadena = "";

//variable que guarda la estacion
int estacion = 0;

//Variable que guarda el tope
int topar = 3;

//Matriz que almacena la matriz del diseño
int matriz[8][8];

//configuracion panel
int pinCS = 10; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int numeroMatricesHorizontales = 4;
int numeroMatricesVerticales = 1;

Max72xxPanel matrizLED = Max72xxPanel(pinCS, numeroMatricesHorizontales, numeroMatricesVerticales);


byte pin_rows[ROW_NUM] = {25, 26, 27, 28}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {22, 23, 24}; //connect to the column pinouts of the keypad

const String password = "1"; // change your password here
String input_password;
String autorizacion = "";

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );
int estado = 1;
String estacion_actual;
bool flag_parcearCadena = false;
int contador_estaciones = 0;
char caracter;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  //pantalla
  lcd.begin(16, 2); //16 columnas y 2 filas
  lcd.setCursor(1, 0);
  lcd.print("  INGRESE SU    ");
  lcd.setCursor(1, 1);
  lcd.print(" USUARIO:    ");
  input_password.reserve(32);

  //Emulacion cadena recibida
  cadena += "00100100";  //Fila 1
  cadena += "00100100";  //Fila 2
  cadena += "00100100";  //Fila 3
  cadena += "00100100";  //Fila 4
  cadena += "00000000";  //Fila 5
  cadena += "11100111";  //Fila 6
  cadena += "00100100";  //Fila 7
  cadena += "00011000";  //Fila 8

  //Configuracion de los paneles
  matrizLED.setIntensity(7);
  matrizLED.setPosition(1, 0, 0);
  matrizLED.setPosition(0, 1, 0);
  matrizLED.setRotation(0, 3);
  matrizLED.setRotation(1, 3);
  matrizLED.setRotation(2, 3);
  matrizLED.setRotation(3, 3);

  //Escribe en los paneles y setea todos apagados
  matrizLED.fillScreen(LOW);
  matrizLED.write();

}

void Estacion() {
  if (Serial.available()) {
    caracter = Serial.read();
    if (caracter == '1') {
      estacion_actual = "1";
    } else if (caracter == '2') {
      estacion_actual = "2";
    } else if (caracter == '3') {
      estacion_actual = "3";
    } else if (caracter == '4') {
      estacion_actual = "4";
      //PENDIENTE BOTON DE EMERGENCIA
    }
  } else {
    estacion_actual = "error";
  }
}

String Recibiendo_Cadena() {
  String text;
  while (Serial.available()) {
    delay(5);
    char c = Serial.read();
    text += c;
  }
  if (text.length() > 0) {
    flag_parcearCadena = true;
    return text;
  }
}

void loop() {
  switch (estado) {
    case 1:
      USUARIO();
      delay(50);
      break;
    case 2:
      cadena = Recibiendo_Cadena();
      delay(50);
      if (flag_parcearCadena == true) {
        parcearCadena(cadena);
      }
      break;
    case 3:
      Estacion();
      if (estacion_actual != "error") {
        serial_Estacion(estacion_actual);
        estacion_actual == "";
        Serial1.println(contador_estaciones);
        if (contador_estaciones == 3) {
          delay(50);
          serial_Estacion("4");
          estacion_actual == "";
          contador_estaciones = 0;
          estado = 2;
          delay(1000);
          matrizLED.fillScreen(LOW);
          matrizLED.write();
        }
      }
      break;
  }
}

void USUARIO() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("  INGRESE SU    ");
  lcd.setCursor(1, 1);
  lcd.print("  USUARIO:    ");

  Verificar_Usuario();
  if (estado == 2) {
    for (int i = 0; i < 25; i++) {
      Serial.println(autorizacion);
      delay(50);
    }
  }
}

void Verificar_Usuario() {
  char key = keypad.getKey();

  if (key) {
    Serial1.print(key);
    if (key == '#') {
      input_password = ""; // clear input password
    } else if (key == '*') {
      if (password == input_password) {
        autorizacion = "Autorizado";
        estado = 2;
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("  Bienvenido   ");
        lcd.setCursor(1, 1);
        lcd.print("             ");
        delay(800);
        lcd.clear();
      } else {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("            ");
        lcd.setCursor(1, 1);
        lcd.print("  ERROR   ");
        autorizacion = "Denegado";
        delay(800);
      }
      input_password = ""; // clear input password
    } else {
      input_password += key; // append new character to input password string
    }
  }
}

//Convierte una cadena string recibida del diseño a matriz
void parcearCadena(String cadenas) {
  int contador = 0;
  for (int a = 0; a < 8; a++) {
    for (int b = 0; b < 8; b++) {
      matriz[a][b] = cadenas[contador];
      contador++;
    }
  }
  flag_parcearCadena = false;
  Serial1.println("Conversion con exito.");
  estado = 3;
}

//Convierte la cadena recibida de la estacion actual a int
void parsearEstacion(String cadena) {
  if (cadena[0] == 49) {
    estacion = 1;
  } else if (cadena[0] == 50) {
    estacion = 2;
  } else if (cadena[0] == 51) {
    estacion = 3;
  } else if (cadena[0] == 52) {
    estacion = 4;
  }
}

//Imprime en el primer panel como parametro recibe hasta donde grafica
void imprimirPanel1(int tope) {
  //Tope = 8 = 100%
  //Tope = 6 = 66%
  //Tope = 3 = 33%

  for (int b = 0; b < 8; b++) {
    for (int a = 0; a < tope; a++) {
      //LOW
      if (matriz[a][b] == 48) {
        matrizLED.drawPixel(b, a, LOW);
        matrizLED.write();
      }
      //HIGH
      else {
        matrizLED.drawPixel(b, a, HIGH);
        matrizLED.write();
      }
    }

  }
}


//Imprime en el segundo panel como parametro recibe hasta donde grafica
void imprimirPanel2(int tope) {
  //Tope = 8 = 100%
  //Tope = 6 = 66%
  //Tope = 3 = 33%

  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < tope; y++) {
      //LOW
      if (matriz[y][x] == 48) {
        matrizLED.drawPixel(x + 8, y, LOW);
        matrizLED.write();
      }
      //HIGH
      else {
        matrizLED.drawPixel(x + 8, y, HIGH);
        matrizLED.write();
      }
    }

  }
}


//Imprime en el tercer panel como parametro recibe hasta donde grafica
void imprimirPanel3(int tope) {
  //Tope = 8 = 100%
  //Tope = 6 = 66%
  //Tope = 3 = 33%

  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < tope; y++) {
      //LOW
      if (matriz[y][x] == 48) {
        matrizLED.drawPixel(x + 16, y, LOW);
        matrizLED.write();
      }
      //HIGH
      else {
        matrizLED.drawPixel(x + 16, y, HIGH);
        matrizLED.write();
      }
    }

  }
}

//Va actualizando el valor de topar para la proxima vez que se necesite
void verificarTopar() {
  if (topar == 6) {
    topar = topar + 2;
  } else if (topar == 8) {
    topar = 3;
  }
  else {
    topar = topar + 3;
  }
}

void limpiarPanel(int numeroPanel) {

  switch (numeroPanel) {
    case 1://Limpiar panel 1
      for (int b = 0; b < 8; b++) {
        for (int a = 0; a < 8; a++) {
          //LOW
          matrizLED.drawPixel(b, a, LOW);
          matrizLED.write();


        }

      }
      break;
    case 2:
      for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
          //LOW
          matrizLED.drawPixel(x + 8, y, LOW);
          matrizLED.write();

        }

      }
      break;
    case 3:
      for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
          //LOW
          matrizLED.drawPixel(x + 16, y, LOW);
          matrizLED.write();

        }

      }
      break;
  }


}

//Panel 4 controla a los otros 3 segun el numero que se ingrese muestra el numero
//En su panel y el panel correspondiente.
void imprimirPanel4(int numero) {
  int x = 26;
  int y = 0;
  int veces = 0;

  switch (numero) {
    case 1: matrizLED.drawChar(x, y, '1', HIGH, LOW, 1);
      matrizLED.write();
      //Ejecutamos la estacion que recibio
      //Aqui hacemos que parpadea

      veces = 0;
      //El numero de veces son la veces que parpadea (veces * 200) = Milisegundos
      while (veces < 8) {
        imprimirPanel1(topar);
        delay(100);

        //Apagamos todos los led
        limpiarPanel(1);
        delay(100);
        veces++;
      }
      verificarTopar();


      //verificarTopar();
      break;
    case 2: matrizLED.drawChar(x, y, '2', HIGH, LOW, 1);
      matrizLED.write();
      //Ejecutamos la estacion que recibio
      //Aqui hacemos que parpadea

      veces = 0;
      //El numero de veces son la veces que parpadea (veces * 200) = Milisegundos
      while (veces < 8) {
        imprimirPanel2(topar);
        delay(100);

        //Apagamos todos los led
        limpiarPanel(2);
        delay(100);
        veces++;
      }
      verificarTopar();
      //verificarTopar();
      break;
    case 3: matrizLED.drawChar(x, y, '3', HIGH, LOW, 1);
      matrizLED.write();
      //Ejecutamos la estacion que recibio
      //Aqui hacemos que parpadea

      veces = 0;
      //El numero de veces son la veces que parpadea (veces * 200) = Milisegundos
      while (veces < 8) {
        imprimirPanel3(topar);
        delay(100);

        //Apagamos todos los led
        limpiarPanel(3);
        delay(100);
        veces++;
      }
      verificarTopar();
      //verificarTopar();
      break;
    case 4:
      matrizLED.fillScreen(LOW);
      matrizLED.write();
      veces = 0;
      for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
          //LOW
          if (matriz[y][x] == 48) {
            matrizLED.drawPixel(x + 24, y, LOW);
            matrizLED.write();
          }
          //HIGH
          else {
            matrizLED.drawPixel(x + 24, y, HIGH);
            matrizLED.write();
          }
        }
      }

      break;
  }
}

void serial_Estacion(String recibir) {
  //Parseamos la estacion que recibimos en el serial
  parsearEstacion(recibir);
  //Llamamos al panel cuatro que muestra el panel correcto
  imprimirPanel4(estacion);
  contador_estaciones++;
}
