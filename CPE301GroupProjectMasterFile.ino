//install the DHTLib library
#include <dht.h> 
//Includes the Arduino Stepper Library
#include <Stepper.h>

#define DHT11_PIN 7 //DHT Pin
#define POWER_PIN 6 //Water Sensor Power Pin
#define SIGNAL_PIN A5 //Water Sensor Analog Signal Pin
#define RDA 0x80
#define TBE 0x20

//Pointer Initializations:
volatile unsigned char *myUCSR0A = (unsigned char *)0x00C0;
volatile unsigned char *myUCSR0B = (unsigned char *)0x00C1;
volatile unsigned char *myUCSR0C = (unsigned char *)0x00C2;
volatile unsigned int  *myUBRR0  = (unsigned int *) 0x00C4;
volatile unsigned char *myUDR0   = (unsigned char *)0x00C6;
 
volatile unsigned char* my_ADMUX = (unsigned char*) 0x7C;
volatile unsigned char* my_ADCSRB = (unsigned char*) 0x7B;
volatile unsigned char* my_ADCSRA = (unsigned char*) 0x7A;
volatile unsigned int* my_ADC_DATA = (unsigned int*) 0x78;

volatile unsigned char *portDDRB = (unsigned char *) 0x04;
volatile unsigned char *portB =    (unsigned char *) 0x05;
volatile unsigned char *pinB =     (unsigned char *) 0x03;

volatile unsigned char *portDDRH = (unsigned char *) 0x101;
volatile unsigned char *portH =    (unsigned char *) 0x102;
volatile unsigned char *pinH =     (unsigned char *) 0x100;

volatile unsigned char *portDDRE = (unsigned char *) 0x2D;
volatile unsigned char *portE =    (unsigned char *) 0x2E;
volatile unsigned char *pinE =     (unsigned char *) 0x2C;

volatile unsigned char *portDDRF = (unsigned char *) 0x30;
volatile unsigned char *portF =    (unsigned char *) 0x31;
volatile unsigned char *pinF =     (unsigned char *) 0x2F;

volatile unsigned char *portDDRG = (unsigned char *) 0x33;
volatile unsigned char *portG =    (unsigned char *) 0x34;
volatile unsigned char *pinG =     (unsigned char *) 0x32;



//Global Variables for DHT Library
dht DHT;

// Defines the number of steps per rotation
const int stepsPerRevolution = 2038;
// Creates an instance of stepper class
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);



//Global Variables for DC Motor
int speedPin = 5;
int dir1 = 4;
int dir2 = 3;
int mSpeed = 90;

// variable to store the Water Sensor value
int value = 0; 

//Global variables for digits




void setup() {

    U0init(9600);
 
  //Setup code for DC Motor

  *portDDRE |= (0x01 << 3); //Sets speed to output on digital pin 5.
  *portDDRG |= (0x01 << 5); //Sets dir1 to output on digital pin 4.
  *portDDRE |= (0x01 << 5); //Sets dir2 to output on digital pine 3. 
 



  //Setup code for Water sensor
  *portDDRH |= (0x01 << 3); 
  *portH &= ~(0x01 << 3);
}


void loop() {

  while (U0kbhit()==0){}; // wait for RDA = true

  executeStepperMotor();
  executeWaterSensor();
  executeDCMotor();
  executeHumiditySensor();

}

void executeStepperMotor(){
 //Execution for Stepper Motor
  // Rotate CW slowly at 5 RPM
  myStepper.setSpeed(5);
  myStepper.step(stepsPerRevolution);
  delay(1000);
  // Rotate CCW quickly at 10 RPM
  myStepper.setSpeed(10);
  myStepper.step(-stepsPerRevolution);
  delay(1000);

}

void executeWaterSensor(){
  //Execution for Water sensor
  *portH |= (0x01 << 3); // turn the sensor ON
  delay(10); // wait 10 milliseconds
  value = analogRead (SIGNAL_PIN); // CONVERT INTO ADC FUNCTION LATER
  *portH &= ~(0x01 << 3); // turn the sensor OFF

   unsigned char aString[14] = {'S', 'e', 'n', 's', 'o', 'r', ' ', 'v', 'a', 'l', 'u', 'e', ':', ' '};
   for(int i = 0; i < 14; i++){
     U0putchar(aString[i]);
   }

  convertNumAndPrint(value);
  delay(1000); 
 
  unsigned char alert[25] = {'W','A', 'R','N','I','N','G',':',' ','W','A','T','E','R',' ','L','E','V','E','L',' ','L','O','W','\n'};
  if(value < 100) {
    for(int i = 0; i < 25; i++){
     U0putchar(alert[i]);
    }
  }
}

void executeDCMotor(){
 //Execution code for DC Motor
  *portDDRG &= ~(0x01 << 5);
  *portDDRE |= (0x01 << 5);
  analogWrite(speedPin, 255); // CONVERT INTO ADC FUNCTION LATER
  delay(25);
  analogWrite(speedPin, mSpeed); //CONVERT INTO ADC FUNCTION LATER
  delay(5000);
}

void executeHumiditySensor(){
  //Execution code for Humidity Sensor
  int chk = DHT.read11(DHT11_PIN);
  Serial.print("Temperature = ");
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
