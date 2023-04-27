//Code for the LCD

#include <LiquidCrystal.h>

int seconds = 0;

LiquidCrystal lcd_1(23, 25, 27, 29, 31, 33);



void setup() {
  // put your setup code here, to run once:
  lcd_1.begin(16,2);

  lcd_1.print("sup bitches");
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd_1.setCursor(0, 1);
  lcd_1.print(seconds);
  delay(1000); 
  seconds += 1;
}
