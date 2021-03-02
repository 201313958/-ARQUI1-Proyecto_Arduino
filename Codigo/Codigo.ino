#include <LiquidCrystal.h>
#include <Keypad.h>

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

byte pin_rows[ROW_NUM] = {25, 26, 27, 28}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {22, 23, 24}; //connect to the column pinouts of the keypad

const String password = "1"; // change your password here
String input_password;
String autorizacion = "";

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

void setup() {
  Serial.begin(9600);
  
  //pantalla
  lcd.begin(16, 2); //16 columnas y 2 filas
  lcd.setCursor(1, 0);
  lcd.print("  INGRESE SU    ");
  lcd.setCursor(1, 1);
  lcd.print(" USUARIO:    ");
  input_password.reserve(32);
}

void loop() {
  USUARIO();
  delay(50);
}

void USUARIO() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("  INGRESE SU    ");
  lcd.setCursor(1, 1);
  lcd.print("  USUARIO:    ");

  Verificar_Usuario();
  Serial.println(autorizacion);
}

void Verificar_Usuario() {
  char key = keypad.getKey();

  if (key) {
    //Serial.print(key);
    if (key == '#') {
      input_password = ""; // clear input password
    } else if (key == '*') {
      if (password == input_password) {
        autorizacion = "Autorizado";
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("  Bienvenido   ");
        lcd.setCursor(1, 1);
        lcd.print("             ");
        delay(800);
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
