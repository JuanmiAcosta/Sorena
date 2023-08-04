#define outputA 6  //Data  (Sentido antihorario)
#define outputB 7  //Clock (Sentido horario)
#define sw 5
#define outputA2 2
#define outputB2 3
#define sw2 12
#define MULTI 8
#define back_func 9
#define sensorPIN A1

LiquidCrystal_I2C lcd(0x27, 20, 4);

float t64 = 0, t250 = 0, t500 = 0, t1000 = 0, t2000 = 0, t4000 = 0, t8000 = 0, t16000 = 0;  //diferentes resultados del t60 para diferentes octavas
float trev60teo = 0;

int aState;  //Para detectar la posición de los pines del encoder respecto a las placas metálicas internas.
int aLastState;

int aState2;  //Para detectar la posición de los pines del encoder respecto a las placas metálicas internas del rotary secundario
int aLastState2;

int dentro = 0;  //para empezar a medir en el t.rev práctico

extern int banderarotary2;

int multi = 0;  //variables usadas en la función de generador de ondas
int multi_ant = 0;
long int counter = 500;
long int counter_ant = 0;
long int function = 0;
long int function_ant = 0;
int func = 0;
int func_ant = 0;

int bandgenton = 0;  //variables bandera de diferentes funciones de los submenús
int primerag = 0;
int bandt60 = 0;
int primerat = 0;
int bandindat = 0;
int primeraindat = 0;

LiquidLine linea10(1, 0, "Med. teorica");  //Se inicializan en el segundo bit para dejar hueco para el cursor (flecha), se crean los objetos antes de utilizarlos.
LiquidLine linea20(1, 1, "Med. practica");
LiquidLine linea30(1, 3, "Soporte");
LiquidScreen pantalla1(linea10, linea20, linea30);

LiquidLine linea11(1, 0, "Insercion de datos");
LiquidLine linea12(1, 1, "Resultado ant.");
LiquidLine linea13(1, 2, "Sonometro");
LiquidLine linea14(1, 3, "Atras");
LiquidScreen pantalla2(linea11, linea12, linea13, linea14);

LiquidLine linea21(1, 0, "Gen. tonos");
LiquidLine linea22(1, 1, "T. Reverberacion");
LiquidLine linea23(1, 2, "Resultado ant.");
LiquidLine linea24(1, 3, "Atras");
LiquidScreen pantalla3(linea21, linea22, linea23, linea24);

LiquidLine linea31(1, 0, "Correo electronico");
LiquidLine linea32(1, 1, "Instagram");
LiquidLine linea33(1, 2, "Linkedin");
LiquidLine linea34(1, 3, "Atras");
LiquidScreen pantalla4(linea31, linea32, linea33, linea34);

LiquidMenu menu(lcd, pantalla1, pantalla2, pantalla3, pantalla4);

void fn_atras();
void fn_t60teo();
void fn_t60pra();
void fn_sop();
void fn_indat();
void fn_resant();
void fn_sonom();
void fn_genton();
void fn_t60();
void fn_reant();
void fn_gmail();
void fn_insta();
void fn_linkedin();

//Funciones generales------------------------------------------------------------------------------------------------------------
void inicializamenu() {

  linea10.set_focusPosition(Position::LEFT);  //Posición de la flecha en el menú (Izquierda)
  linea20.set_focusPosition(Position::LEFT);
  linea30.set_focusPosition(Position::LEFT);

  linea10.attach_function(1, fn_t60teo);  // Se le añade una función a la selección de cada línea
  linea20.attach_function(1, fn_t60pra);
  linea30.attach_function(1, fn_sop);

  menu.add_screen(pantalla1);  //Añadimos las pantalla al menú

  linea11.set_focusPosition(Position::LEFT);
  linea12.set_focusPosition(Position::LEFT);
  linea13.set_focusPosition(Position::LEFT);
  linea14.set_focusPosition(Position::LEFT);

  linea11.attach_function(1, fn_indat);
  linea12.attach_function(1, fn_resant);
  linea13.attach_function(1, fn_sonom);
  linea14.attach_function(1, fn_atras);

  menu.add_screen(pantalla2);

  linea21.set_focusPosition(Position::LEFT);
  linea22.set_focusPosition(Position::LEFT);
  linea23.set_focusPosition(Position::LEFT);
  linea24.set_focusPosition(Position::LEFT);

  linea21.attach_function(1, fn_genton);
  linea22.attach_function(1, fn_t60);
  linea23.attach_function(1, fn_reant);
  linea24.attach_function(1, fn_atras);

  menu.add_screen(pantalla3);

  linea31.set_focusPosition(Position::LEFT);
  linea32.set_focusPosition(Position::LEFT);
  linea33.set_focusPosition(Position::LEFT);
  linea34.set_focusPosition(Position::LEFT);

  linea31.attach_function(1, fn_gmail);
  linea32.attach_function(1, fn_insta);
  linea33.attach_function(1, fn_linkedin);
  linea34.attach_function(1, fn_atras);

  menu.add_screen(pantalla4);

  pantalla1.set_displayLineCount(3);
  pantalla2.set_displayLineCount(4);
  pantalla3.set_displayLineCount(4);
  pantalla4.set_displayLineCount(4);
}


//Funciones generales------------------------------------------------------------------------------------------------------------

void selectOption()  //para seleccionar las opciones con el encoder principal.
{
  if (digitalRead(sw) == LOW) {
    menu.call_function(1);
    delay(500);
  }
}

void fn_atras()  //para volver en el menú principal
{
  menu.change_screen(1);
  menu.set_focusedLine(0);
}

//Funciones pantalla 1------------------------------------------------------------------------------------------------------------

void fn_t60teo()  //para moverse por las pantallas del menú principal
{
  menu.change_screen(2);
  menu.set_focusedLine(0);
}

void fn_t60pra() {
  menu.change_screen(3);
  menu.set_focusedLine(0);
}

void fn_sop() {
  menu.change_screen(4);
  menu.set_focusedLine(0);
}
//Funciones pantalla 2------------------------------------------------------------------------------------------------------------

void fn_indat() {

  bandindat = 1;
  primeraindat = 1;
  Serial.println("Primera INDAT");
}

void fn_resant()  //muestra resultados por pantalla
{

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("  T. Reverberacion  ");
  lcd.setCursor(0, 1);
  lcd.print("      teorico      ");
  lcd.setCursor(7, 3);
  lcd.print(trev60teo);
  lcd.print(" s");
  delay(4000);

  menu.change_screen(2);
  menu.set_focusedLine(0);
}

void fn_sonom() {

  const int sampleWindow = 50;  // Ancho ventana en mS (50 mS = 20Hz)
  unsigned long startMillis = millis();
  unsigned int Max = 0;
  unsigned int Min = 1024;
  // Recopilar durante la ventana
  unsigned int sample;
  int contadorsonom = 0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.println("     Sonometro      ");
  lcd.setCursor(0, 2);
  lcd.print("(          )  xxxx   ");

  while (contadorsonom < 10) {

    while (millis() - startMillis < sampleWindow) {


      Serial.println("Medidas");
      sample = analogRead(sensorPIN);

      if (sample < 1024) {

        if (sample > Max) {

          Max = sample;  // Actualizar máximo
        }

        else if (sample < Min) {

          Min = sample;  // Actualizar mínimo
        }
      }
    }
    Serial.println(Max);
    Serial.println(Min);
    Serial.println("Imprimiendo");
    unsigned int picoapico = Max - Min;       // Amplitud del sonido
    double volts = (picoapico * 5.0) / 1024;  // Convertir a tensión
    double db = (20. * log(10)) * (volts);    //Convertir a dB


    lcd.setCursor(0, 0);
    lcd.println("     Sonometro      ");
    lcd.setCursor(0, 1);
    lcd.println("                    ");

    lcd.setCursor(0, 3);
    lcd.print("Sen.sonora(dB)=");
    lcd.print(db);
    lcd.setCursor(19, 3);
    lcd.print(" ");
    /* lcd.setCursor(18,3);
                lcd.print("   ");*/
    Serial.println(db);
    Serial.println("dB");

    if ((db >= 0) && (db <= 10)) {
      lcd.setCursor(0, 2);
      lcd.print("(.         )  BAJA   ");
    }

    if ((db > 10) && (db <= 20)) {
      lcd.setCursor(0, 2);
      lcd.print("(..        )  BAJA   ");
    }

    if ((db > 20) && (db <= 30)) {
      lcd.setCursor(0, 2);
      lcd.print("(...       )  BAJA   ");
    }

    if ((db > 30) && (db <= 40)) {
      lcd.setCursor(0, 2);
      lcd.print("(....      )  MEDIA   ");
    }

    if ((db > 40) && (db <= 50)) {
      lcd.setCursor(0, 2);
      lcd.print("(.....     )  MEDIA   ");
    }

    if ((db > 50) && (db <= 60)) {
      lcd.setCursor(0, 2);
      lcd.print("(......    )  MEDIA   ");
    }

    if ((db > 60) && (db <= 70)) {
      lcd.setCursor(0, 2);
      lcd.print("(.......   )  ALTA   ");
    }

    if ((db > 70) && (db <= 80)) {
      lcd.setCursor(0, 2);
      lcd.print("(........  )  ALTA   ");
    }

    if ((db > 80) && (db <= 90)) {
      lcd.setCursor(0, 2);
      lcd.print("(......... )  ALTA   ");
    }

    if ((db > 90) && (db <= 100)) {
      lcd.setCursor(0, 2);
      lcd.print("(..........) M.ALTA    ");
    }

    Serial.println("Medidas");
    contadorsonom++;
    delay(1000);

    startMillis = millis();
    Max = 0;
    Min = 1024;
    Serial.println(contadorsonom);
  }

  menu.change_screen(2);
  menu.set_focusedLine(0);
  contadorsonom == 0;
}

//Funciones pantalla 3------------------------------------------------------------------------------------------------------------

void fn_genton() {

  bandgenton = 1;
  primerag = 1;
  Serial.println("Primera Genton");
}

void fn_t60() {

  lcd.clear();
  bandt60 = 1;
  primerat = 1;
  dentro = 0;
  //mensaje pulsa la tecla multi para empezar la medida
  do {
    lcd.setCursor(4, 1);
    lcd.print("PULSE MULTI");
    Serial.println("Pulse MULTI");
    //esperando la pusacion de una tecla
  } while (digitalRead(MULTI) != LOW);
  //genero tono
  dentro = 1;
  Serial.println("A");
  //mido

  //imprimo resultados
}

void fn_reant() {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T.Rev.64: ");
  lcd.print(t64);
  lcd.print("s");
  lcd.setCursor(0, 1);
  lcd.print("T.Rev.250: ");
  lcd.print(t250);
  lcd.print("s");
  lcd.setCursor(0, 2);
  lcd.print("T.Rev.500: ");
  lcd.print(t500);
  lcd.print("s");
  lcd.setCursor(0, 3);
  lcd.print("T.Rev.1000: ");
  lcd.print(t1000);
  lcd.print("s");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T.Rev.2000: ");
  lcd.print(t2000);
  lcd.print("s");
  lcd.setCursor(0, 1);
  lcd.print("T.Rev.4000: ");
  lcd.print(t4000);
  lcd.print("s");
  lcd.setCursor(0, 2);
  lcd.print("T.Rev.8000: ");
  lcd.print(t8000);
  lcd.print("s");
  lcd.setCursor(0, 3);
  lcd.print("T.Rev.16000: ");
  lcd.print(t16000);
  lcd.print("s");
  delay(3000);

  menu.change_screen(3);
  menu.set_focusedLine(0);
}

//Funciones pantalla 4------------------------------------------------------------------------------------------------------------

void fn_gmail() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("     sorena2021     ");
  lcd.setCursor(0, 1);
  lcd.print("     @gmail.com     ");
  lcd.setCursor(14, 3);
  lcd.print("Ver1.0");
  delay(3000);
  menu.change_screen(4);
  menu.set_focusedLine(0);
}

void fn_insta() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("       <User>       ");
  lcd.setCursor(0, 1);
  lcd.print("    @Sorena2021     ");
  lcd.setCursor(14, 3);
  lcd.print("Ver1.0");
  delay(3000);
  menu.change_screen(4);
  menu.set_focusedLine(0);
}

void fn_linkedin() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("       <User>       ");
  lcd.setCursor(0, 1);
  lcd.print("   Juan Miguel     ");
  lcd.setCursor(0, 2);
  lcd.print("   Acosta Ortega   ");
  lcd.setCursor(14, 3);
  lcd.print("Ver1.0");
  delay(3000);
  menu.change_screen(4);
  menu.set_focusedLine(0);
}
