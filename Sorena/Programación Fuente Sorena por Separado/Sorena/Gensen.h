#define FSYNC 38

#define WAVE_SINE     0x2000
#define WAVE_SQUARE   0x2028
#define WAVE_TRIANGLE 0x2002

#define outputA 6 //Data  (Sentido antihorario)
#define outputB 7 //Clock (Sentido horario)
#define sw 5
#define MULTI 8

#define sw2 12
#define outputA2 2
#define outputB2 3


void AD9833setup();
void AD9833reset();
void AD9833setFrequency(long frequency, int Waveform);
void WriteRegister(int dat);

void limpiaDigitos();



void AD9833setup() //de nuevo escrituras de registro y setup del AD9833
{
   pinMode(FSYNC, OUTPUT);
   digitalWrite(FSYNC, HIGH);
   SPI.begin();
   delay(50); 
   AD9833reset();
}

void AD9833reset()
{
   WriteRegister(0x100);
   delay(10);
}



void WriteRegister(int dat)
{ 
   SPI.setDataMode(SPI_MODE2);        
   digitalWrite(FSYNC, LOW);
   delayMicroseconds(10); 
   SPI.transfer(dat>>8);
   SPI.transfer(dat&0xFF);
   digitalWrite(FSYNC, HIGH);
   SPI.setDataMode(SPI_MODE0);
}

void limpiaDigitos() //limpia los dígitos sobrantes en pantalla
{
   if(counter < 100)
   {
      lcd.setCursor(2, 0);
      lcd.print("      ");  
   }else
   {
      if(counter < 1000)
      {
         lcd.setCursor(3, 0);
         lcd.print("     "); 
      }else
      {
         if(counter < 10000)
         {
            lcd.setCursor(4, 0);
            lcd.print("    "); 
         }else
         {
            if(counter < 100000)
            {
               lcd.setCursor(5, 0);
               lcd.print("   "); 
            }else
            {
               if(counter < 1000000)
               {
                  lcd.setCursor(6, 0);
                  lcd.print("  "); 
               }else
               {
                  if(counter < 10000000)
                  {
                     lcd.setCursor(7, 0);
                     lcd.print(" "); 
                  }
               }
            }
         }
      }                
   }
}
