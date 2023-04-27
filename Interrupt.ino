const byte ledPin = 13 ;

const byte interruptPin = 2 ;

volatile byte state = LOW ;

void setup ( ) {

   pinMode ( ledPin , OUTPUT ) ;

   pinMode ( interruptPin , INPUT_PULLUP) ;

   attachInterrupt ( digitalPinToInterrupt ( interruptPin ), glow, CHANGE ) ;

} 

void loop ( ) {

   digitalWrite ( ledPin, state );

}

void glow ( ) 

{

   state = !state ;

}