/*
 This code will blink an LED attached to pin 13 on and off. 
 It will stay on for 0.25 seconds.
 It will stay off for 1 second.
 */


#include <Metro.h> //Include Metro library
#define LED 13 // Define the led's pin

//Create a variable to hold theled's current state
int state = HIGH;

int sensor = 0;


// Instanciate a metro object and set the interval to 250 milliseconds (0.25 seconds).
Metro ledMetro = Metro(200); 
Metro ledWait = Metro(800); 

void setup()
{
    Serial.begin(115200);
  pinMode(LED,OUTPUT);
  digitalWrite(LED,state);
}

void loop()
{

  sensor = touchRead(T0);


  Serial.println(sensor);
  

  if (sensor >= 40) {
    LEDPLAY();
  }
    if (sensor <= 15) {
    LEDWAIT();
  }
  
}


void LEDPLAY () {
    if (ledMetro.check() == 1) { // check if the metro has passed its interval .
    if (state==LOW)  state=HIGH;
    else state=LOW;
    
    digitalWrite(LED,state);
  }
}

void LEDWAIT (){ 
      if (ledWait.check() == 1) { // check if the metro has passed its interval .
    if (state==LOW)  state=HIGH;
    else state=LOW;
    
    digitalWrite(LED,state);
  }
}

