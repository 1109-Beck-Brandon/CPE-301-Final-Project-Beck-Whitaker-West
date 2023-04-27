//Humidity code that displays to the LCD

#include <dht.h> //install the DHTLib library
#include <LiquidCrystal.h> //Add the LCD Library

dht DHT;
#define DHT11_PIN 7

#define RDA 0x80
#define TBE 0x20

int seconds = 0;

LiquidCrystal lcd_1(23, 25, 27, 29, 31, 33);

//Pointer Initializations:
volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;

void setup() {
  // put your setup code here, to run once:
  U0init(9600);

  lcd_1.begin(16,2);
}

void loop() {
  // put your main code here, to run repeatedly:
  //Execution code for Humidity Sensor
  int chk = DHT.read11(DHT11_PIN);
  unsigned char bString[14] = {'T', 'e', 'm', 'p', 'e', 'r', 'a', 't', 'u', 'r', 'e', ' ', '=', ' '};
   for(int i = 0; i < 14; i++){
     U0putchar(bString[i]);
   }

   convertNumAndPrint(DHT.temperature);

  unsigned char cString[11] = {'H', 'u', 'm', 'i', 'd', 'i', 't', 'y', ' ', '=', ' '};
  for(int i = 0; i < 11; i++){
    U0putchar(cString[i]);
  }
  
  convertNumAndPrint(DHT.humidity);
  delay(1000);

  lcd_1.setCursor(0, 0);
  lcd_1.print("Temp: ");
  lcd_1.print(DHT.temperature);
  lcd_1.setCursor(0, 1);
  lcd_1.print("Humidity: ");
  lcd_1.print(DHT.humidity);
  delay(1000);
}

void U0init(unsigned long U0baud)
{

 unsigned long FCPU = 16000000;
 unsigned int tbaud;
 tbaud = (FCPU / 16 / U0baud - 1);
 // Same as (FCPU / (16 * U0baud)) - 1;
 *myUCSR0A = 0x20;
 *myUCSR0B = 0x18;
 *myUCSR0C = 0x06;
 *myUBRR0  = tbaud;
}

// Read USART0 RDA status bit and return non-zero true if set

unsigned char U0kbhit()
{
  if(*myUCSR0A & RDA){
    return true;
  }
  else{
    return false;
  }


// Read input character from USART0 input buffer
}
unsigned char U0getchar()
{
  unsigned char ch;
  while(!(UCSR0A & (1<<RXC0)));
  ch =  *myUDR0;
  return ch;
  
  
}

// Wait for USART0 TBE to be set then write character to transmit buffer
void U0putchar(unsigned char U0pdata)
{

while((*myUCSR0A & (TBE)) == 0); 
*myUDR0 = U0pdata;

}

void convertNumAndPrint(int num){

  unsigned char cs1, cs2, cs3; 
  unsigned char csArray[3];
  
  int index = 0, test = 0;

  if(num <= 9 && num > 0){
    csArray[0] = num + '0';
    index = 1;
    }

    else if(num <= 99 && num > 9){
      csArray[1] = (num % 10) + '0';
      csArray[0] = (num /10) + '0';
      index = 2;
    }

    else if(num <= 999 && num > 99){
      csArray[1] = (num % 10) + '0';
      test = (num / 100);
      csArray[2] = (test / 10) + '0';
      csArray[0] = (num / 100) + '0';
      index = 3;
    }

    else{
      index = 0;
    }
  
    for(int i = 0; i < index; i++){
      U0putchar(csArray[i]);
    }
      U0putchar('\n');
      index = 0;
}
