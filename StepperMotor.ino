#include <Stepper.h> //Includes the Arduino Stepper Library
// Defines the number of steps per rotation
const int stepsPerRevolution = 2038;
// Creates an instance of stepper class
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);

#define SIGNAL_PIN A0

#define RDA 0x80
#define TBE 0x20

//Pointer Initializations:
volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;

//Create the variable for the potentiometer reading
int sensorValue = 0;

void setup() {
  // put your setup code here, to run once:
  // Nothing to do (Stepper Library sets pins as outputs)

  U0init(9600);

  pinMode(A0, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

  // read the value from the sensor
  sensorValue = analogRead(A0);
  
  if(sensorValue <= 50) {
    // Rotate CW slowly at 5 RPM
    myStepper.setSpeed(10);
    myStepper.step(stepsPerRevolution);
  }
  else if(sensorValue >= 1000) {
    //Rotate CCW quickly at 10 RPM
    myStepper.setSpeed(10);
    myStepper.step(-stepsPerRevolution);
  }
  
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
