#include <dht.h> //install the DHTLib library
//Includes the Arduino Stepper Library
#include <Stepper.h>

#define DHT11_PIN 7 //DHT Pin
#define POWER_PIN 6 //Water Sensor Power Pin
#define SIGNAL_PIN A5 //Water Sensor Analog Signal Pin

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




void setup() {
 
  //Setup code for DC Motor
  pinMode(speedPin, OUTPUT);
  pinMode(dir1, OUTPUT);
  pinMode(dir2, OUTPUT);
  Serial.begin(9600);

  //Setup code for Water sensor
  pinMode (POWER_PIN, OUTPUT); // configure D7 pin as an OUTPUT
  digitalWrite (POWER_PIN, LOW); // turn the sensor OFF
}


void loop() {

  //Execution code for DC Motor
  digitalWrite(dir1, LOW);
  digitalWrite(dir2, HIGH);
  analogWrite(speedPin, 255);
  delay(25);
  analogWrite(speedPin, mSpeed);
  delay(5000);

  //Execution code for Humidity Sensor
  int chk = DHT.read11(DHT11_PIN);
  Serial.print("Temperature = ");
  Serial.println(DHT.temperature);    
  Serial.print("Humidity = ");
  Serial.println(DHT.humidity);
  delay(1000);

  //Execution for Stepper Motor
  // Rotate CW slowly at 5 RPM
  myStepper.setSpeed(5);
  myStepper.step(stepsPerRevolution);
  delay(1000);
  // Rotate CCW quickly at 10 RPM
  myStepper.setSpeed(10);
  myStepper.step(-stepsPerRevolution);
  delay(1000);

  //Execution for Water sensor
  digitalWrite (POWER_PIN, HIGH); // turn the sensor ON
  delay(10); // wait 10 milliseconds
  value = analogRead (SIGNAL_PIN); // read the analog value from sensor
  digitalWrite (POWER_PIN, LOW); // turn the sensor OFF
  Serial.print("Sensor value: " );
  Serial.println (value);
  delay(1000);
}
