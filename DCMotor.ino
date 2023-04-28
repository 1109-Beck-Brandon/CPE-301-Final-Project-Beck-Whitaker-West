//Working DC Motor w/ UART Implementation

//Pointer Initializations:
volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;

volatile unsigned char *portDDRE = (unsigned char *) 0x2D;
volatile unsigned char *portE =    (unsigned char *) 0x2E;
volatile unsigned char *pinE =     (unsigned char *) 0x2C;

volatile unsigned char *portDDRG = (unsigned char *) 0x33;
volatile unsigned char *portG =    (unsigned char *) 0x34;
volatile unsigned char *pinG =     (unsigned char *) 0x32;

int speedPin = 5;
int dir1 = 4;
int dir2 = 3;
int mSpeed = 90;

#define RDA 0x80
#define TBE 0x20

void setup() {
  // put your setup code here, to run once:
  U0init(9600);
 
  //Setup code for DC Motor

  *portDDRE |= (0x01 << 3); //Sets speed to output on digital pin 5.
  *portDDRG |= (0x01 << 5); //Sets dir1 to output on digital pin 4.
  *portDDRE |= (0x01 << 5); //Sets dir2 to output on digital pin 3.
}

void loop() {
  // put your main code here, to run repeatedly:

  //Execution code for DC Motor
  *portG &= ~(0x01 << 5);
  *portE |= (0x01 << 5);
  analogWrite(speedPin, 2000); // CONVERT INTO ADC FUNCTION LATER
  delay(25);
  analogWrite(speedPin, mSpeed); //CONVERT INTO ADC FUNCTION LATER
  delay(5000);
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
