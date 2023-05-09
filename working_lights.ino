// Set up the pin connections
const int buttonPin = 13; // connect the button to digital pin 2
const int ledPin = A14; 
const int ledPin2 = A12;  // connect an LED to digital pin 13

// Set up variables to store the button state, LED state, and previous button state
int buttonState = 0;
int ledState = LOW;
int prevButtonState = LOW;

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
}

void loop() {
  // Read the state of the button
  buttonState = digitalRead(buttonPin);

  // Check if the button has been pressed
  if (buttonState == HIGH && prevButtonState == LOW) {
    // Toggle the state of the LED
    ledState = !ledState;
  }

  // Update the LED state
  digitalWrite(ledPin, ledState);
  digitalWrite(ledPin2, !ledState);

  // Update the previous button state
  prevButtonState = buttonState;
}

