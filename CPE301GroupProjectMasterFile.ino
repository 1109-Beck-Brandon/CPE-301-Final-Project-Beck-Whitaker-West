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

void executeStepperMotor();
void executeWaterSensor();
void executeDCMotor();
void executeHumiditySensor();



void setup() {
 
  //Setup code for DC Motor
  pinMode(speedPin, OUTPUT);
  *portDDRE |= (0x01 << 3); //Sets speed to output on digital pin 5.
  *portDDRG |= (0x01 << 5); //Sets dir1 to output on digital pin 4.
  *portDDRE |= (0x01 << 5); //Sets dir2 to output on digital pine 3. 
 

  Serial.begin(9600);

  //Setup code for Water sensor
  *portDDRH |= (0x01 << 3); 
  *portH &= ~(0x01 << 3);
}


void loop() {

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
  Serial.print("Sensor value: " ); // CONVERT INTO UART LATER
  Serial.println (value);
  delay(1000); 
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
  Serial.println(DHT.temperature);    
  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);
  delay(1000);
}
