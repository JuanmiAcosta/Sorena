#include <LiquidCrystal_I2C.h>
#include <LiquidMenu.h>
#include <SPI.h>
#include <MD_cmdProcessor.h>
#include <MD_AD9833.h>

#include "clmenu.h"
#include "Gensen.h"

//Bandera del rotary encoder secundario
int banderarotary2=0;  

//valores usados en el tiempo de reverberación práctico
long int valor[20],mayor,menor;  //tiempo[500];

int dat;   //Variable necesaria para el generador de ondas

bool pulsant=LOW, pulsnueva=LOW, pulsacion=0,seleccion=0; //Para controlar el flanco de bajada, flanco de subida, y opciones del t60 teorico

unsigned long inicio=0, fin=0, transcurrido=0;//variables para obtener el paso del tiempo en la funcion de t.rev. practico

long int paso=9; //variable bandera para el tiempo de reverberación teórico

int contador=0; //para la inserción de dígitos

float volumen=0,areasuelo=0,areapared13=0,areapared24=0,areatecho=0,ancho=0,largo=0,alto=0,areapared=0 ,coefabsuelo=0;
float coefabpared=0,coefabtecho=0,area=0,areaabsuelo=0,areaabparedes=0,areaabtecho=0,contadorfloat=0, areaventana=0,areaabventana=0,coefabventana=0;
//variables usadas en el cálculo teórico del T60

void selectOption();//llamamos a todas las funciones en el archivo principal
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
void inicializamenu();

void AD9833setup();
void AD9833reset();
void AD9833setFrequency(long frequency, int Waveform);
void WriteRegister(int dat);
void actualiza_func();
void limpiaDigitos();

void setup() 
{
   Serial.begin(9600);
   pinMode(FSYNC, OUTPUT); //escrituras de registro e inserción de valores necesarias para el generador de tonos AD9833 encontradas en su datasheet
   digitalWrite(FSYNC, HIGH);
   SPI.begin();
   delay(50); 
   WriteRegister(0x100);
   delay(10);
   SPI.setDataMode(SPI_MODE2);        
   digitalWrite(FSYNC, LOW);
   delayMicroseconds(10); 
   SPI.transfer(dat>>8);
   SPI.transfer(dat&0xFF);
   digitalWrite(FSYNC, HIGH);
   SPI.setDataMode(SPI_MODE0);

  pinMode(sw,INPUT_PULLUP);
  pinMode(back_func,INPUT_PULLUP);
  pinMode(MULTI,INPUT_PULLUP);
  pinMode(sw2,INPUT_PULLUP);
 
  lcd.begin();
  lcd.backlight();

  lcd.clear();
  
  lcd.setCursor(7,0); lcd.print("Sorena");
  lcd.setCursor(0,2); lcd.print("Ver 0.1    ");
  
  lcd.setCursor(0,3);
  for( int i=0 ; i<20 ; i++ )
  {
    lcd.print(".");
    delay(50);
  }
  delay(50);
  menu.init();  //Esta inicialización solo es necesaria si se usa el módulo I2C
  inicializamenu();
  menu.set_focusedLine(0);
  menu.update();
  AD9833setup();
}

void loop() 
{

  selectOption();
//rotary del menu
  aState = digitalRead(outputA); 
    if (aState != aLastState)
    {     
      if (digitalRead(outputB) != aState) 
      { 
        
       menu.switch_focus(true);
      } else {
        
       menu.switch_focus(false); 
      }
      menu.update();
      aLastState = aState;
  }
  
//rotary para cambiar datos
    aState2 = digitalRead(outputA2); 
    if (aState2 != aLastState2)
    {     
      if (digitalRead(outputB2) != aState2) 
      { 
        banderarotary2=-1;
        Serial.println("sentido antihorario");
        Serial.println(banderarotary2);
       
      } else 
      {
        banderarotary2=1;
        Serial.println("sentido horario");
        Serial.println(banderarotary2);
        
      }
      aLastState2 = aState2;
  }
  
  pulsnueva=digitalRead(sw2);
if (pulsant==HIGH && pulsnueva==LOW && (bandindat!=0 || dentro==1 || bandt60!=0))//flanco de bajada
{
  pulsant=LOW;
  delay(50);
  pulsacion=0;
  banderarotary2=0;
}

if (pulsant==LOW && pulsnueva==HIGH && (bandindat!=0 || dentro==1))//flanco de subida, que solo funcione en funciones concretas.
{
  pulsant=HIGH;
  delay(50);
  pulsacion=1;
  banderarotary2=0;
}

//variable bandera para entrar a la función de generador de tonos
  if(bandgenton!=0)
  {

         //Para que sólo se ejecute esta fracción de código una sola vez por entrada a la función
    if(primerag==1)
    {
      counter=500;
      func=0;
      digitalWrite(FSYNC, HIGH);
      lcd.clear();
      lcd.setCursor(0,0);
      primerag=0;
    }
    //Leo el multiplicador de frecuencia (botón MULTI)
    if(!digitalRead(MULTI))
    { 
         delay(50);
         if(!digitalRead(MULTI))
         { 
            multi=multi+1;
            
            Serial.println("Multi -> ");Serial.println(multi);
            if(multi>=7) multi=0;
         }
      }
      
     //leo el rotary para modificar al frecuencia
    if (banderarotary2!=0)
    {
        if(banderarotary2>0)
        { 
          counter=counter+pow(10,multi);
          if(counter>100000) counter=1000000;
          banderarotary2=0;
          Serial.println("frecuencia=  ");
          Serial.println(counter);
        }
        if(banderarotary2<0)
        {
          counter=counter-pow(10,multi);
          if(counter<0) counter=0;
          banderarotary2=0;
          Serial.println("frecuencia=  ");
          Serial.println(counter);
        }
    }
    //Leo el pulsador para cambiar el tipo de onda
    if(!digitalRead(sw2))
    {
      delay(100);
      if(!digitalRead(sw2))
      { 
        
         if(func_ant == 0){
            func = 1;
            Serial.println("sw1º");}
         if(func_ant == 1){
            func = 2;
            Serial.println("sw2º");}
         if(func_ant == 2){
            func = 0;
            Serial.println("sw3º");}
                  
      }
   }

    //si hay alguna modificación en cualquiera de estos valores se actualiza la onda y la pantalla, antes no.
    if((counter_ant != counter) || (function_ant != function) || (multi!=multi_ant) || (func_ant != func))
    {
      lcd.setCursor(0, 3);
      lcd.print("10^");
      lcd.print(multi);
      lcd.setCursor(17, 0);
      lcd.print("Hz");
      lcd.setCursor(0, 0);
      lcd.print(counter);
      
      Serial.println("Escribiendo...");
      
      if(func == 0){
      lcd.setCursor(10, 3);
      lcd.print("  SENOIDAL");
      function = WAVE_SINE;
      }
      if(func == 1){
      lcd.setCursor(10, 3);
      lcd.print("TRIANGULAR");
      function = WAVE_TRIANGLE;
      }
      if(func == 2){
      lcd.setCursor(10, 3);
      lcd.print("  CUADRADA");
      function = WAVE_SQUARE;
      }
      Serial.println("Funcion");
      Serial.println(function,HEX);
      limpiaDigitos();
      AD9833setFrequency(counter, function);
      Serial.println("Actualizo");
      
      counter_ant = counter;
      function_ant = function;
      multi_ant=multi;
      func_ant = func;
      }
          
   //Leo el pulsador para salir de este menu
  if(digitalRead(back_func)==LOW)
  {  
     digitalWrite(FSYNC, LOW);
     counter=0;
     menu.change_screen(3);
     menu.set_focusedLine(0);
     Serial.println("Salir genton");
     bandgenton=0;
     AD9833setFrequency(counter, function);
  }
 }

 //en este momento se entra a la funcion de tiempo de reverberación práctico
  if(bandt60!=0)
  {
   
    if(primerat==1) //hacemos esto cada vez que se entra a la función
    { 
      lcd.clear();
      primerat=0;  
    }

  if(dentro==1)
{  
  //Esta función no esta terminada, pues después de hacer pruebas con el ADC nativo y el ADS1115 de Adafruit se determinó que hacía falta uno de mucha más calidad, y no había tiempo para realizar más pruebas.
  lcd.setCursor(1,0);
  lcd.print("  Manten VOLVER   ");
  Serial.println("1000Hz");
  
  counter=1000;
  function=WAVE_SINE;
  AD9833setFrequency(counter, function);
  delay(1000);
  
  counter=0;
  AD9833setFrequency(counter, function);

/*  
  for(int i=0;i<200;i++)
  {
  valor[i] = analogRead(sensorPIN);
  tiempo[i] = millis();
  
  if (valor < menor) menor=valor;
  if (valor > mayor) mayor=valor;
  }
*/
  
  if(digitalRead(back_func)==LOW) //para poder salir y reestablecer ciertos valores a la normalidad
  {  
     
     menu.change_screen(3);
     menu.set_focusedLine(0);
     Serial.println("Salir t60");
     bandt60=0;
     dentro=0;
     primerat=1;
     counter=0;
     function=WAVE_SINE;
     AD9833setFrequency(counter, function);
  
  }
 }
}

 //entramos a la inserción de datos; casi todas las funciones tienen su variable bandera
  if (bandindat!=0)
  {   
    
    if(primeraindat==1) //paso a paso se introducen los valores
    { 
      lcd.clear();
      paso=9;
      pulsacion=0;
      lcd.setCursor(0,0);
      lcd.print("Insercion datos:");
      lcd.setCursor(0,1);
      lcd.print("Ancho= ");
      primeraindat=0;
    }

    if (pulsacion==1 && paso==9)
      { 
      paso=8;
      ancho=contador-0.5;
      Serial.println("ancho=");  
      Serial.println(ancho);
      contador=0;
      lcd.setCursor(0,2);
      lcd.print("Largo= ");
      Serial.println("ancho=contador");  
      
       
      pulsacion=0;
       }
      

    if ((banderarotary2!=0) && (paso==9) && (pulsacion==0))
    {
      lcd.setCursor(0,1);
      lcd.print("Ancho= ");
      lcd.setCursor(9,1);
      lcd.print(contador);
      lcd.setCursor(14,1);
      lcd.print("m");
      Serial.println("Ancho");
      Serial.println(contador);
      
        if(banderarotary2>0)
        { 
          contador=contador+0.5;
          banderarotary2=0;
        }
        if(banderarotary2<0)
        {
          contador=contador-0.5;
          banderarotary2=0;
          if(contador<=0) contador=0;
        }
       banderarotary2=0; 
    }

    if (pulsacion==1 && paso ==8)
    {  
      paso=7;
      largo=contador-0.5;
      Serial.println("largo=");  
      Serial.println(largo);
      contador=0; 
      pulsacion=0; 
      lcd.setCursor(0,3);
      lcd.print("Alto= ");
    }

   if ((banderarotary2!=0) && (paso==8))
    {
      Serial.println("Paso");
      Serial.println(paso); 
      lcd.setCursor(0,2);
      lcd.print("Largo= ");
      lcd.setCursor(9,2);
      lcd.print(contador);
      lcd.setCursor(14,2);
      lcd.print("m");
      Serial.println("largo");
      Serial.println(contador);
       
        if(banderarotary2>0)
        { 
          contador=contador+0.5;
          banderarotary2=0;
        }
        if(banderarotary2<0)
        {
          contador=contador-0.5;
          banderarotary2=0;
          if(contador<=0) contador=0;
        }
    }

    if (pulsacion==1 && paso ==7)
     { 
      paso=6;
      alto=contador-0.5;
      Serial.println("ancho");Serial.println(ancho);
      Serial.println("largo");Serial.println(largo);
      Serial.println("alto");Serial.println(alto);
      volumen=alto*ancho*largo;
      areapared13=2*(largo*alto);
      areapared24=2*(ancho*alto);
      areapared=areapared13+areapared24;
      areasuelo=ancho*largo;
      areatecho=ancho*largo;
      contador=0;
      delay(20); 
      lcd.clear(); 
      lcd.setCursor(0,1);
      lcd.print("Coef. ab suelo= ");
      Serial.println("volumen");Serial.println(volumen);
      Serial.println("pared");Serial.println(areapared);
      Serial.println("suelo");Serial.println(areasuelo);
      Serial.println("techo");Serial.println(areatecho);
      pulsacion=0;
      }
    
   if ((banderarotary2!=0) && (paso==7))
    {
      lcd.setCursor(0,3);
      lcd.print("Alto= ");
      lcd.setCursor(9,3);
      lcd.print(contador);
      lcd.setCursor(14,3);
      lcd.print("m");
      
      Serial.println(contador);
      
        if(banderarotary2>0)
        { 
          contador=contador+0.5;
          banderarotary2=0;
        }
        if(banderarotary2<0)
        {
          contador=contador-0.5;
          banderarotary2=0;
          if(contador<=0) contador=0;
        }
    }

    if (pulsacion==1 && paso ==6)
    {  
      paso=5;
      coefabsuelo=contadorfloat-0.01;
      areaabsuelo=coefabsuelo*areasuelo;
      contadorfloat=0; 
      pulsacion=0;
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("Coef. ab. pared= "); 
      }

   if ((banderarotary2!=0) && (paso==6))
    {
      Serial.println("Paso");
      Serial.println(paso); 
      lcd.setCursor(0,2);
      lcd.print(contadorfloat);
      
      Serial.println("c. a. suelo");
      Serial.println(contadorfloat);
      
      
        if(banderarotary2>0)
        { 
          contadorfloat=contadorfloat+0.01;
          banderarotary2=0;
          
        }
        if(banderarotary2<0)
        {
          contadorfloat=contadorfloat-0.01;
          banderarotary2=0;
          if(contadorfloat<=0) contadorfloat=0;
        }
    }

    if (pulsacion==1 && paso ==5)
    { 
      paso=4;
      coefabpared=contadorfloat-0.01;
      contadorfloat=0;
      delay(20);
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print("Coef. ab. techo= "); 
      pulsacion=0; 
      }

   if ((banderarotary2!=0) && (paso==5))
    {

      lcd.setCursor(0,2);
      lcd.print(contadorfloat);
      Serial.println(contadorfloat);
      
        if(banderarotary2>0)
        { 
          contadorfloat=contadorfloat+0.01;
          banderarotary2=0;
        }
        if(banderarotary2<0)
        {
          contadorfloat=contadorfloat-0.01;
          banderarotary2=0;
          if(contadorfloat<=0) contadorfloat=0;
        }
    }

    if (pulsacion==1 && paso ==4)
    {  
      paso=3;
      coefabtecho=contadorfloat-0.01;
      areaabtecho=coefabtecho*areatecho;
      contadorfloat=0;  
      pulsacion=0;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Alguna pared dispar:");
          lcd.setCursor(1,1);
          lcd.print("SI");
          lcd.setCursor(6,1);
          lcd.print("NO");
      Serial.println("c.a suelo");Serial.println(coefabsuelo);
      Serial.println("c.a techo");Serial.println(coefabtecho);
      Serial.println("c.a pared");Serial.println(coefabpared);
      }

   if ((banderarotary2!=0) && (paso==4))
    {
      Serial.println("Paso");
      Serial.println(paso); 

      lcd.setCursor(0,2);
      lcd.print(contadorfloat);
      
      Serial.println("a");
      Serial.println(contadorfloat);
      
       if(banderarotary2>0)
        { 
          contadorfloat=contadorfloat+0.01;
          banderarotary2=0;
          
        }
        if(banderarotary2<0)
        {
          contadorfloat=contadorfloat-0.01;
          banderarotary2=0;
          if(contadorfloat<=0) contadorfloat=0;
        }
    }

    if (pulsacion==1 && paso ==3 && seleccion==1)
    {
      paso=2;
      banderarotary2=0;
      pulsacion=0;
      contador=0;
      contadorfloat=0;
      Serial.println("si");
      lcd.setCursor(0,2);
      lcd.print("Superficie= ");
    }

    if (pulsacion==1 && paso==3 && seleccion==0)
    {
      paso=0;
      banderarotary2=0;
      pulsacion=0;
      contadorfloat=0;
      contador=0;
      Serial.println("no");
    }
     
  if ((banderarotary2!=0) && (paso==3))
  {
           if(banderarotary2>0)
        { 
          lcd.setCursor(0,1);
          lcd.print(">SI   NO  ");
          seleccion=1;
          banderarotary2=0;  
        }
        if(banderarotary2<0)
        {
          lcd.setCursor(0,1);
          lcd.print(" SI  >NO  ");
          areaabparedes=coefabpared*areapared;
          seleccion=0;
          banderarotary2=0; 
        }
  }

  if (pulsacion==1 && paso==2)
  {
    paso=1;
    areaventana=contador-0.5;
    areapared=areapared-areaventana;
    areaabparedes=coefabpared*areapared;
    pulsacion=0;
    contador=0;
    contadorfloat=0;
    lcd.setCursor(0,3);
    lcd.print("co.ab.dispar=  ");
  }

    if ((banderarotary2!=0) && (paso==2))
    {
      lcd.setCursor(12,2);
      lcd.print(contador);
      lcd.setCursor(17,2);
      lcd.print("m^2");

         if(banderarotary2>0)
        { 
          contador=contador+0.5;
          banderarotary2=0;
        }
        if(banderarotary2<0)
        {
          contador=contador-0.5;
          banderarotary2=0;
          if(contador<=0) contador=0;
        }
      
    }

  if (pulsacion==1 && paso==1)
  {
    paso=0;
    coefabventana=contador-0.01;
    areaabventana=coefabventana*areaventana;
    areaabparedes=areaabparedes+areaabventana;
    pulsacion=0;
    contador=0;
    contadorfloat=0;
  }

   if ((banderarotary2!=0) && (paso==1))
    {
      lcd.setCursor(13,3);
      lcd.print(contador);

         if(banderarotary2>0)
        { 
          contador=contador+0.01;
          banderarotary2=0;
        }
        if(banderarotary2<0)
        {
          contador=contador-0.01;
          banderarotary2=0;
          if(contador<=0) contador=0;
        }
      
    }
  
  if (paso==0)
    { 
      Serial.println("areaabtecho");Serial.println(areaabtecho);
      Serial.println("areaabsuelo");Serial.println(areaabsuelo);
      Serial.println("areaabparedes");Serial.println(areaabparedes);
      
      area=areaabtecho+areaabparedes+areaabsuelo;
      trev60teo=(0.161*volumen)/area;
      Serial.println("volumen");Serial.println(volumen);
      Serial.println("area");Serial.println(area);
      Serial.println("trevt60teo");Serial.println(trev60teo);
      lcd.clear();
      lcd.print("  T. Reverberacion  ");
      lcd.setCursor(0,1);
      lcd.print("      teorico      ");
      lcd.setCursor(7,3);
      lcd.print(trev60teo); lcd.print(" s");
      delay(4000);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Insercion datos:");
      lcd.setCursor(0,1);
      lcd.print("Ancho= ");
      paso=9;
    }

    if(digitalRead(back_func)==LOW)
  {   
     menu.change_screen(2);
     menu.set_focusedLine(0);
     Serial.println("salir indat");
     bandindat=0;
     paso=9;
  }
 } 
}


void AD9833setFrequency(long frequency, int Waveform)  //funcion que establece la frecuencia y forma de onda necesaria en el archivo principal.
 {
   long FreqWord = (frequency * pow(2, 28)) / 25.0E6;
   int MSB = (int)((FreqWord & 0xFFFC000) >> 14);
   int LSB = (int)(FreqWord & 0x3FFF);
   LSB |= 0x4000;
   MSB |= 0x4000; 
   WriteRegister(0x2100);   
   WriteRegister(LSB);
   WriteRegister(MSB);
   WriteRegister(0xC000);
   WriteRegister(Waveform);
 }
