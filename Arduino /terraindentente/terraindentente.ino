#include <CapacitiveSensor.h>


/*
   CapitiveSense Library Demo Sketch
   Paul Badger 2008
   Uses a high value resistor e.g. 10M between send pin and receive pin
   Resistor effects sensitivity, experiment with values, 50K - 50M. Larger resistor values yield larger sensor values.
   Receive pin is the sensor pin - try different amounts of foil/metal on this pin
*/

/*Este código construye sobre el codigo explorado y realizado durante la maestria. Green V4

*/

/**El pin de la derecha es el pin que recibe, el pin de la izquierda es el sensor


  PINS A4 / A5 / 0 / 1 / 13 CANNOT BE USED FOR CAPACITIVE SENSING - or at least doesnt seem like it  (ARDUINO)

  Capacitance needs to be tested before hand to have a SensorMax that comes close to the
  actual range to then establish the dynamic calibration

  v.4  Inclusion de LEDs para que las personas tengan un cue visual de la interaccion - using timers to display different behaviors
*/

//Declaración de sensores
CapacitiveSensor   plant1 = CapacitiveSensor(12, A0);       // 10M resistor between pins 2 & 3, pin 3 is sensor pin, add a wire and or foil if desired
CapacitiveSensor   plant2 = CapacitiveSensor(12, A1);       // 10M resistor between pins 2 & 4, pin 4 is sensor pin, add a wire and or foil
CapacitiveSensor   plant3 = CapacitiveSensor(12, A2);       // 10M resistor between pins 2 & 7, pin 7 is sensor pin, add a wire and or foil if desired
CapacitiveSensor   plant4 = CapacitiveSensor(12, A3);        // 10M resistor between pins 2 & 10, pin 10 is sensor pin, add a wire and or foil

//Es necesario dividir los sensores porque sino se jode y solo captan bien las primeras cuatro plantas - hice igual con la baranda
CapacitiveSensor   plant5 = CapacitiveSensor(2, 10);        // 10M resistor between pins 2 & 12, pin 12 is sensor pin, add a wire and or foil
CapacitiveSensor   plant6 = CapacitiveSensor(2, 11);        // 10M resistor between pins 2 & 12, pin 12 is sensor pin, add a wire and or foil
CapacitiveSensor   plant7 = CapacitiveSensor(2, 5);        // 10M resistor between pins 2 & 12, pin 12 is sensor pin, add a wire and or foil
CapacitiveSensor   plant8 = CapacitiveSensor(2, 8);

//Initial calibration
int sensorMin[] = {12000, 12000, 12000, 12000, 12000, 12000, 12000, 12000}; // Minimum sensor value > ATTENTION be sure to use a limit that is close to what is being sensed by the plants
int sensorMax[8];     // Maximum sensor value
int sensorValue[8];

//Filtering
int filtered[8];
int Previousfiltered[8];

//Create a variable to hold theled's current state
int state = LOW;

int leds [8] = {A4, A5, 9, 13, 3, 4, 6, 7};

unsigned long previousMillis[8]; //[x] = number of leds


int ledplay [8];


/*So, no PWMs but gonna use this to set the interval of on-off to have live feedback of plant - human interaction */

//Normalized value
float Value[8];

//Mapping algorithm - normalization
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


/* En realidad en este momento solo necesito que las plantas funcionen como botones asi que
  es un array para guardar el -pin- que en teoria estarian activando
  ya que los shields no funcionaron voy a manejar el audio con touch, asi que funcionara como
  la ultima vez, una variable para guardar el numero de archivo que deberia estar sonando
*/

int triggerPin;

void setup()
{
  Serial.begin(115200);


  for (int i = 0 ; i < 8; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], state);
  }


  Serial.println("Starting calibration for 10 seconds");
  // calibrate during the first n seconds
  while (millis() < 10000)
  {
    long p1 =  plant1.capacitiveSensor(30);
    long p2 =  plant2.capacitiveSensor(30);
    long p3 =  plant3.capacitiveSensor(30);
    long p4 =  plant4.capacitiveSensor(30);
    long p5 =  plant5.capacitiveSensor(30);
    long p6 =  plant6.capacitiveSensor(30);
    long p7 =  plant7.capacitiveSensor(30);
    long p8 =  plant8.capacitiveSensor(30);

    long sensorPlant[] = {p1, p2, p3, p4, p5, p6, p7, p8};

    for (int i = 0 ; i < 8; i++) {
      sensorValue[i] = sensorPlant[i];
    }

    for (int i = 0 ; i < 8; i++) {
      if (sensorValue[i] > sensorMax[i])    // save the maximum sensor value found
      {
        sensorMax[i] = sensorValue[i];
      }

      if (sensorValue[i] < sensorMin[i])    // save the minimum sensor value found
      {
        sensorMin[i] = sensorValue[i];
      }
    }
  }


  Serial.println("Print results");

  while (millis() < 15000)
  {
    Serial.print("Minimos");
    Serial.print(" ");

    for (int i = 0 ; i < 8; i++) {
      Serial.print(sensorMin[i]);
      Serial.print(" ");
    }

    Serial.print("Maximos");
    Serial.print(" ");

    for (int i = 0 ; i < 8; i++) {

      Serial.print(sensorMax[i]);
      Serial.print(" ");
    }

    Serial.println(" ");
  }
  delay(200);

  Serial.println("Finished calibration");
}

void loop()
{

  //long start = millis();
  long p1 =  plant1.capacitiveSensor(30);
  long p2 =  plant2.capacitiveSensor(30);
  long p3 =  plant3.capacitiveSensor(30);
  long p4 =  plant4.capacitiveSensor(30);
  long p5 =  plant5.capacitiveSensor(30);
  long p6 =  plant6.capacitiveSensor(30);
  long p7 =  plant7.capacitiveSensor(30);
  long p8 =  plant8.capacitiveSensor(30);


  long sensorPlant[] = {p1, p2, p3, p4, p5, p6, p7, p8};


  // Capacitance filtered value

  for (int i = 0 ; i < 8; i++) {
    filtered[i] = filtered[i] * 0.95 + sensorPlant[i] * 0.05;
  }

  for (int i = 0 ; i < 8; i++) {
    sensorValue[i] = filtered[i];
  }

  // Cálculo de la diferencia  entre la lectura actual y la lectura previa para ajustar el valor máximo captado
  for (int i = 0 ; i < 8; i++) {
    if ((abs(filtered[i] - Previousfiltered[i]) >= 250)) {
      sensorMax[i] = sensorValue[i];
    }
  }

  //Map - normalize the captured values
  for (int i = 0 ; i < 8; i++) {
    Value[i] = mapfloat(sensorValue[i], sensorMin[i], sensorMax[i], 4000, 10);
  }

  for (int i = 0 ; i < 8; i++) {
    Value[i] = constrain(Value[i], 10, 4000);
  }

  for (int i = 0 ; i < 8; i++) {
    ledplay[i] = Value[i];
  }

  //  for (int i = 0 ; i < 8; i++) {
  BlinkLed(leds[0], ledplay[0], 0);
  BlinkLed(leds[1], ledplay[1], 1);
  BlinkLed(leds[2], ledplay[2], 2);
  BlinkLed(leds[3], ledplay[3], 3);
  BlinkLed(leds[4], ledplay[4], 4);
  BlinkLed(leds[5], ledplay[5], 5);
  BlinkLed(leds[6], ledplay[6], 6);
  BlinkLed(leds[7], ledplay[6], 7);
  //}


  /*Implementación de porcentajes para establecer un threshold de activación dinámico ya que estoy utilizando valores dinámicos para el min y el máx
    de los sensores debido al posible cambio de afluencia de participantes. En un principio pensé que se el threshold debía establecerse con el mínimo.
    Sin embargo, debido a que la capacitancia fluctua todo el tiempo, y los sensores son altamente sensibles, el
    porcentaje al que debe responder es a sí mismo*/



  //If 20% over previous reading activate
  /*for (int i = 0 ; i < 8; i++) {
    if (filtered[i] >= (Previousfiltered[i] + (Previousfiltered[i] * 0.20))) {
      triggerPins [i] = 1;
    }*/

  //No se porque no se esta asignando correctamente si lo meto en un array asi que toca uno por uno

  if (filtered[0] >= (Previousfiltered[0] + (Previousfiltered[0] * 0.20))) {
    triggerPin = 1;
  }

  if (filtered[1] >= (Previousfiltered[1] + (Previousfiltered[1] * 0.20))) {
    triggerPin = 2;
  }

  if (filtered[2] >= (Previousfiltered[2] + (Previousfiltered[2] * 0.20))) {
    triggerPin = 3;
  }

  if (filtered[3] >= (Previousfiltered[3] + (Previousfiltered[3] * 0.20))) {
    triggerPin  = 4;
  }

  if (filtered[4] >= (Previousfiltered[4] + (Previousfiltered[4] * 0.20))) {
    triggerPin  = 5;
  }

  if (filtered[5] >= (Previousfiltered[5] + (Previousfiltered[5] * 0.20))) {
    triggerPin = 6;
  }

  if (filtered[6] >= (Previousfiltered[6] + (Previousfiltered[6] * 0.20))) {
    triggerPin = 7;
  }

  if (filtered[7] >= (Previousfiltered[7] + (Previousfiltered[7] * 0.20))) {
    triggerPin = 8;
  }




  /*Sending all the data to TD both to visualize it and think of how it's gonna be used in the system. So in order
    for the data to be used in TD I will prepend it's name followed by a '=' that will then be taken away*/

  //Serial print to be read in TD - no use of " " because TD will automatically divide the incoming data by the space

  Serial.print("ActivePin=");  // prints the value read
  Serial.print(triggerPin);
  Serial.print("=");

  Serial.print("\t");


  ///*
  Serial.print("SensorValue=");
  //Serial.print(" ");

  for (int i = 0 ; i < 8; i++) {
    Serial.print(sensorValue[i]);
    Serial.print("=");
  }
  Serial.print("\t"); // use this instead of println to output only one delimiter - println outputs \r\n

  Serial.print("SensorMax=");
  //Serial.print(" ");

  for (int i = 0 ; i < 8; i++) {

    Serial.print(sensorMax[i]);
    Serial.print("=");
  }

  Serial.print("\t");

  Serial.print("LedInterval=");
  // Serial.print(" ");

  for (int i = 0 ; i < 8; i++) {

    Serial.print(ledplay[i]);
    Serial.print("=");
  }

  Serial.print("\n");


  /*
    Serial.print(Value[0]);                  // print sensor output 1
    Serial.print("\t");
    Serial.print(Value[1]);                  // print sensor output 2
    Serial.print("\t");
    Serial.print(Value[2]);                  // print sensor output 2
    Serial.print("\t");
    Serial.print(Value[3]);                  // print sensor output 2
    Serial.print("\t");
    Serial.println(Value[4]);                // print sensor output 3
    //*/

  /*
    Serial.print(millis() - start);        // check on performance in milliseconds
    Serial.print("\t");                    // tab character for debug windown spacing


    Serial.print(p1);                  // print sensor output 1
    Serial.print("\t");
    Serial.print(p2);                  // print sensor output 2
    Serial.print("\t");
    Serial.print(p3);                  // print sensor output 2
    Serial.print("\t");
    Serial.print(p4);                  // print sensor output 2
    Serial.print("\t");
    Serial.println(p5);                // print sensor output 3
  */

  /*
    RESET  ---------------------------------------------------------------------

    Resetting the variables is really important! don't forget - otherwise the state
    keeps adding and the filtered and previous filtered siempre son iguales*/

  triggerPin = 0;



  //Reset of previous filtered

  for (int i = 0 ; i < 8; i++) {
    Previousfiltered[i] = filtered[i];
  }

  delay(150);                          // arbitrary delay to limit data to serial port
}


//Function by FrabizioP — http://www.instructables.com/id/Blink-multiple-Leds-1-Function-No-Delay/

void BlinkLed (int led, int timer, int array) {

  //(long) can be omitted if you dont plan to blink led for very long time I think
  if (((long)millis() - previousMillis[array]) >= timer) {

    previousMillis[array] = millis(); //stores the millis value in the selected array

    digitalWrite(led, !digitalRead(led)); //changes led state
  }
}




