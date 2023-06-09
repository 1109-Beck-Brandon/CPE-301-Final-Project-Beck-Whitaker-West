#include <Wire.h>
#include <RTClib.h>

//install the DHTLib library
#include <dht.h> 
//Includes the Arduino Stepper Library
#include <Stepper.h>
 //Add the LCD Librar
#include <LiquidCrystal.h>


#define DHT11_PIN 7 //DHT Pin
#define POWER_PIN 6 //Water Sensor Power Pin
#define SIGNAL_PIN A0 //Vent Control Signal Pin for Stepper Motor
#define SIGNAL_PIN A5 //Water Sensor Analog Signal Pin
#define RDA 0x80
#define TBE 0x20


//Pointer Initializations:
volatile unsigned char *myTCCR1A = (unsigned char *) 0x80;
volatile unsigned char *myTCCR1B = (unsigned char *) 0x81;
volatile unsigned char *myTCCR1C = (unsigned char *) 0x82;
volatile unsigned char *myTIMSK1 = (unsigned char *) 0x6F;
volatile unsigned int  *myTCNT1  = (unsigned  int *) 0x84;
volatile unsigned char *myTIFR1 =  (unsigned char *) 0x36;

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

volatile unsigned char *portDDRA = (unsigned char *) 0x21;
volatile unsigned char *portA =    (unsigned char *) 0x22;
volatile unsigned char *pinA =     (unsigned char *) 0x20;

volatile unsigned char *portDDRK = (unsigned char *) 0x107;
volatile unsigned char *portK =    (unsigned char *) 0x108;
volatile unsigned char *pinK =     (unsigned char *) 0x106;




//Global Variables for DHT Library
dht DHT;

// Defines the number of steps per rotation
const int stepsPerRevolution = 2038;
// Creates an instance of stepper class
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);

//Create the variable for the potentiometer reading
int sensorValue = 0;

LiquidCrystal lcd_1(23, 25, 27, 29, 31, 33);




RTC_DS1307 rtc;

//Global Variables for DC Motor
int speedPin = 5;
int dir1 = 4;
int dir2 = 3;
int mSpeed = 90;
int progState = 0;
const int buttonPin = 13; // connect the button to digital pin 2
const int ledPin = A14; 
const int ledPin2 = A12;  // connect an LED to digital pin 13

int buttonState = 0;
int ledState = LOW;
int prevButtonState = LOW;



// variable to store the Water Sensor value
int value = 0;


//Global variables for digits




void setup() {

    U0init(9600);
    Serial.begin(9600);
    adc_init();
    lcd_1.begin(16,2);
 
   //Set Analog Pin 15 to Output. 
   *portDDRK |= (0x01 << 7);
   //Set Analog Pin 14 to Output. 
   *portDDRK |= (0x01 << 6);
   //Set Analog Pin 13 to Output.
   *portDDRK |= (0x01 << 5);
   //Set Analog Pin 12 to Output. 
   *portDDRK |= (0x01 << 4);

    // Initialize the RTC module
    rtc.begin();

    unsigned char rtcString[18] = {'R', 'T', 'C', ' ', 'i', 's', ' ', 'n', 'o', 't', ' ', 'r', 'u', 'n', 'n', 'i', 'n', 'g'};
    if (!rtc.isrunning()) {
      for(int i = 0; i < 18; i++){
       U0putchar(rtcString[i]);
      }
    // Uncomment the following line to set the RTC to the date and time this sketch was compiled
    rtc.adjust(DateTime(2023,5,9,4,00));
    }
    
     

  //Setup code for DC Motor

  *portDDRE |= (0x01 << 3); //Sets speed to output on digital pin 5.
  *portDDRG |= (0x01 << 5); //Sets dir1 to output on digital pin 4.
  *portDDRE |= (0x01 << 5); //Sets dir2 to output on digital pine 3. 
 



  //Setup code for Water sensor
  *portDDRH |= (0x01 << 3); 
  *portH &= ~(0x01 << 3);

  //Set button as input.
  //pinMode(13, INPUT);
  *portDDRB &= ~(0x01 << 7);

  //Set LEDs as OUTPUT.
  //pinMode(22, OUTPUT);
  *portDDRA |= (0x01 << 0); 
  //pinMode(24, OUTPUT);
  *portDDRA |= (0x01 << 2); 
  //pinMode(26, OUTPUT);
  *portDDRA |= (0x01 << 4); 
  //pinMode(28, OUTPUT);
  *portDDRA |= (0x01 << 6); 

}





void loop() {

  //Serial.println("Test");
 
  //while (U0kbhit()==0){}; // wait for RDA = true

  monitorButton();

  

  if(progState == 0){
    idleState();
    
  }

  else if(progState == 1){
    runningState();
    
  }

  else if(progState == 2){
    disabledState();
    
  }

  else if(progState == 3){
    errorState();
    
  }

}

void monitorButton(){

 // Read the state of the button
  
  buttonState = *pinB;

  // Check if the button has been pressed
  if ((*pinB & 0x01) && prevButtonState == LOW) {
    // Toggle the state of the LED
    ledState = !ledState;
  

  // Update the LED state
    if (ledState == HIGH) {
      progState = 2;
      *portK |= (0x01 << 6);
      *portK &= ~(0x01 << 4);
      
    }

    else if (ledState == LOW) {
      progState = 0;
      *portK &= ~(0x01 << 6);
      *portK |= (0x01 << 4);
    }
  }
  // Update the previous button state
  prevButtonState = buttonState;
}

void errorState(){

  executeWaterSensor();
  executeHumiditySensor();

  //Turn on Red LED
  *portK |= (0x01 << 7);
  *portK &= ~(0x01 << 6);
  *portK &= ~(0x01 << 5);
  *portK &= ~(0x01 << 4);
  
  


  //Serial.println("In the error state.");
  unsigned char errorString[20] = {'I', 'n', ' ', 't', 'h', 'e', ' ', 'e', 'r', 'r', 'o', 'r', ' ', 's', 't', 'a', 't', 'e', '.', '\n'};
      for(int i = 0; i < 20; i++){
       U0putchar(errorString[i]);
      }
}

void idleState(){
  executeWaterSensor();
  executeHumiditySensor();

  //Turn on Green LED
  *portK &= ~(0x01 << 7);
  *portK &= ~(0x01 << 5);


  //Serial.println("In the idle state.");
  unsigned char idleString[19] = {'I', 'n', ' ', 't', 'h', 'e', ' ', 'i', 'd', 'l', 'e', ' ', 's', 't', 'a', 't', 'e', '.', '\n'};
      for(int i = 0; i < 19; i++){
       U0putchar(idleString[i]);
      }
}
  

void runningState(){

  executeStepperMotor();
  executeDCMotor();

  executeWaterSensor();
  executeHumiditySensor();

  //Serial.println("In the running state.");
  unsigned char runningString[22] = {'I', 'n', ' ', 't', 'h', 'e', ' ', 'r', 'u', 'n', 'n', 'i', 'n', 'g', ' ', 's', 't', 'a', 't', 'e', '.', '\n'};
      for(int i = 0; i < 22; i++){
       U0putchar(runningString[i]);
      }

  
  ///Turn on Blue LED
  *portK &= ~(0x01 << 7);
  *portK &= ~(0x01 << 6);
  *portK |= (0x01 << 5);
  *portK &= ~(0x01 << 4);


  


}

void disabledState(){

  

  //Turn on Yellow LED
  *portK &= ~(0x01 << 7);
  *portK &= ~(0x01 << 5);

  //Serial.println("In the disabled state.");
  unsigned char disabledString[23] = {'I', 'n', ' ', 't', 'h', 'e', ' ', 'd', 'i', 's', 'a', 'b', 'l', 'e', 'd', ' ', 's', 't', 'a', 't', 'e', '.', '\n'};
      for(int i = 0; i < 23; i++){
       U0putchar(disabledString[i]);
      }
}


void executeStepperMotor(){
 
  

  //Execution for Stepper Motor
  // read the value from the sensor
  sensorValue = adc_read(0);
  
    myStepper.setSpeed(10);
    myStepper.step(-stepsPerRevolution);
  
  if(sensorValue <= 50) {
    // Rotate CW slowly at 5 RPM
    myStepper.setSpeed(10);
    myStepper.step(stepsPerRevolution);
    printRTC();
  }
  

}

void executeWaterSensor(){
  //Execution for Water sensor
  *portH |= (0x01 << 3); // turn the sensor ON
  my_delay(10000); // wait 10 milliseconds

   value = adc_read(5);
  *portH &= ~(0x01 << 3); // turn the sensor OFF

   unsigned char aString[14] = {'S', 'e', 'n', 's', 'o', 'r', ' ', 'v', 'a', 'l', 'u', 'e', ':', ' '};
   for(int i = 0; i < 14; i++){
     U0putchar(aString[i]);
   }

  convertNumAndPrint(value);


  unsigned char alert[25] = {'W','A', 'R','N','I','N','G',':',' ','W','A','T','E','R',' ','L','E','V','E','L',' ','L','O','W','\n'};
   
   if(value < 50) {
    for(int i = 0; i < 25; i++){
     U0putchar(alert[i]);
     
    }
     if(ledState == 0){
       progState = 3;
       printRTC();
     }

     
   } 

 

}

void executeDCMotor(){
  //TimeStamp

  //Execution code for DC Motor
  *portDDRG &= ~(0x01 << 5);
  *portDDRE |= (0x01 << 5);
}


void executeHumiditySensor(){
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

   
   if(DHT.temperature > 22){
     if(ledState == 0){
       progState = 1;
       printRTC();
     }
   }   
    
   
   

    convertNumAndPrint(DHT.humidity);

  lcd_1.setCursor(0, 0);
  lcd_1.print("Temp: ");
  lcd_1.print(DHT.temperature);
  lcd_1.setCursor(0, 1);
  lcd_1.print("Humidity: ");
  lcd_1.print(DHT.humidity);
  

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

    else if(num <= 9999 && num > 999){
      csArray[3] = (num / 1000) + '0';
      test = num / 100;
      csArray[2] = (test % 10) + '0';
      test = num / 10;
      csArray[1] = (test % 10) + '0';
      test = num % 100;
      csArray[0] = (test % 10) + '0';
      index = 4;
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

void adc_init()
{
  // setup the A register
  *my_ADCSRA |= 0b10000000; // set bit   7 to 1 to enable the ADC
  *my_ADCSRA &= 0b11011111; // clear bit 6 to 0 to disable the ADC trigger mode
  *my_ADCSRA &= 0b11110111; // clear bit 5 to 0 to disable the ADC interrupt
  *my_ADCSRA &= 0b11111000; // clear bit 0-2 to 0 to set prescaler selection to slow reading
  // setup the B register
  *my_ADCSRB &= 0b11110111; // clear bit 3 to 0 to reset the channel and gain bits
  *my_ADCSRB &= 0b11111000; // clear bit 2-0 to 0 to set free running mode
  // setup the MUX Register
  *my_ADMUX  &= 0b01111111; // clear bit 7 to 0 for AVCC analog reference
  *my_ADMUX  |= 0b01000000; // set bit   6 to 1 for AVCC analog reference
  *my_ADMUX  &= 0b11011111; // clear bit 5 to 0 for right adjust result
  *my_ADMUX  &= 0b11100000; // clear bit 4-0 to 0 to reset the channel and gain bits
}

unsigned int adc_read(unsigned char adc_channel_num)
{
  // clear the channel selection bits (MUX 4:0)
  *my_ADMUX  &= 0b11100000;
  // clear the channel selection bits (MUX 5)
  *my_ADCSRB &= 0b11110111;
  // set the channel number
  if(adc_channel_num > 7)
  {
    // set the channel selection bits, but remove the most significant bit (bit 3)
    adc_channel_num -= 8;
    // set MUX bit 5
    *my_ADCSRB |= 0b00001000;
  }
  // set the channel selection bits
  *my_ADMUX  += adc_channel_num;
  // set bit 6 of ADCSRA to 1 to start a conversion
  *my_ADCSRA |= 0x40;
  // wait for the conversion to complete
  while((*my_ADCSRA & 0x40) != 0);
  // return the result in the ADC data register
  return *my_ADC_DATA;
}

void printRTC(){

 // Get the current date and time from the RTC module
  DateTime now = rtc.now();

  // Print the date and time to the Serial Monitor
  convertNumAndPrint(now.year());
  U0putchar('/');
  convertNumAndPrint(now.month());
  U0putchar('/');
  convertNumAndPrint(now.day());
  U0putchar(' ');
  convertNumAndPrint(now.hour());
  U0putchar(':');
  convertNumAndPrint(now.minute());
  U0putchar(':');
  convertNumAndPrint(now.second());

}

void my_delay(unsigned int freq)
{
  // calc period
  double period = 1.0/double(freq);
  // 50% duty cycle
  double half_period = period/ 2.0f;
  // clock period def
  double clk_period = 0.0000000625;
  // calc ticks
  unsigned int ticks = half_period / clk_period;
  // stop the timer
  *myTCCR1B &= 0xF8;
  // set the counts
  *myTCNT1 = (unsigned int) (65536 - ticks);
  // start the timer
  * myTCCR1B |= 0b00000001;
  // wait for overflow
  while((*myTIFR1 & 0x01)==0); // 0b 0000 0000
  // stop the timer
  *myTCCR1B &= 0xF8;   // 0b 0000 0000
  // reset TOV           
  *myTIFR1 |= 0x01;
}