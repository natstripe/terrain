#include <CapacitiveSensor.h>

/*
 * CapitiveSense Library Demo Sketch
 * Paul Badger 2008
 * Uses a high value resistor e.g. 10M between send pin and receive pin
 * Resistor effects sensitivity, experiment with values, 50K - 50M. Larger resistor values yield larger sensor values.
 * Receive pin is the sensor pin - try different amounts of foil/metal on this pin
 */

/*Este código construye sobre los ejemplos de CapacitiveSense Library que se encuentra en la página de Arduino Playground.
La idea de este código es utilizar múltiples entradas (sensores/plantas) y probar diferentes valores de resistencias (entre 30M / 48M)
El siguiente paso será integrar poco a poco todos los otros pedazos de código para consolidar GreenDreams*/

//El pin de la derecha es el pin que recibe, el pin de la izquierda es el sensor, NO UTILIZAR EL PIN 0 CON ESTA LIBRERIA - it creates issues
//and stalls the programm from the start

CapacitiveSensor   plant1 = CapacitiveSensor(2, 3);       // 48M resistor between pins 2 & 3, pin 3 is sensor pin, add a wire and or foil if desired
CapacitiveSensor   plant2 = CapacitiveSensor(2, 4);       // 40M resistor between pins 2 & 4, pin 4 is sensor pin, add a wire and or foil
CapacitiveSensor   plant3 = CapacitiveSensor(2, 7);       // 30M resistor between pins 2 & 7, pin 7 is sensor pin, add a wire and or foil if desired
CapacitiveSensor   plant4 = CapacitiveSensor(2, 10);        // 34M resistor between pins 2 & 10, pin 10 is sensor pin, add a wire and or foil
CapacitiveSensor   plant5 = CapacitiveSensor(2, 12);        // 48M resistor between pins 2 & 12, pin 12 is sensor pin, add a wire and or foil

int sensorMin[] = {12000,12000,12000,12000,12000};  // Minimum sensor value > ATTENTION be sure to use a limit that is close to what is being sensed by the plants
int sensorMax[5];     // Maximum sensor value
int sensorValue[5];

float Value[5];


int filtered[5];
int Previousfiltered[5];

//running calibration
int running_min[5];
int running_max[5];


float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void setup()
{
  //cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
  Serial.begin(9600);
  Serial.println("Starting calibration for 10 seconds");
  // calibrate during the first ten seconds
  while (millis() < 10000)
  {
    long p1 =  plant1.capacitiveSensor(30);
    long p2 =  plant2.capacitiveSensor(30);
    long p3 =  plant3.capacitiveSensor(30);
    long p4 =  plant4.capacitiveSensor(30);
    long p5 =  plant5.capacitiveSensor(30);

    long sensorPlant[] = {p1, p2, p3, p4, p5};

    for (int i = 0 ; i < 5; i++) {
      sensorValue[i] = sensorPlant[i];
    }

    for (int i = 0 ; i < 5; i++) {
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
  // calibrate during the first five seconds
  while (millis() < 20000)
  {
    for (int i = 0 ; i < 5; i++) {
      Serial.print("Minimos");
      Serial.print(" ");
      Serial.print("value");
      Serial.print(" ");
      Serial.print(sensorMin[i]);                  // print sensor output 1
    /*  Serial.print("\t");
      Serial.print(sensorMin[1]);                  // print sensor output 2
      Serial.print("\t");
      Serial.print(sensorMin[2]);                  // print sensor output 2
      Serial.print("\t");
      Serial.print(sensorMin[3]);                  // print sensor output 2
      Serial.print("\t");
      Serial.print(sensorMin[4]);
      Serial.print("\t");
      Serial.print(sensorMin[5]); */
      Serial.print(" ");
      Serial.print("Maximos");
      Serial.print(" ");
      Serial.print("value");
      Serial.print(" ");
      Serial.print(sensorMax[i]);                  // print sensor output 1
     /* Serial.print("\t");
      Serial.print(sensorMax[1]);                  // print sensor output 2
      Serial.print("\t");
      Serial.print(sensorMax[2]);                  // print sensor output 2
      Serial.print("\t");
      Serial.print(sensorMax[3]);                  // print sensor output 2
      Serial.print("\t");
      Serial.print(sensorMax[4]);
      Serial.print("\t");
      Serial.print(sensorMax[5]); */
            Serial.print(" ");
      Serial.println("\t");
    }
    delay(20);
  }
  Serial.println("Finished calibration");
}

void loop()
{
  long start = millis();
  long p1 =  plant1.capacitiveSensor(30);
  long p2 =  plant2.capacitiveSensor(30);
  long p3 =  plant3.capacitiveSensor(30);
  long p4 =  plant4.capacitiveSensor(30);
  long p5 =  plant5.capacitiveSensor(30);

  long sensorPlant[] = {p1, p2, p3, p4, p5};
  
  // Capacitance filtered value

  for (int i = 0 ; i < 5; i++) {
    filtered[i] = filtered[i] * 0.95 + sensorPlant[i] * 0.05;
  }

  for (int i = 0 ; i < 5; i++) {
    sensorValue[i] = filtered[i];
  }

  // Cálculo de la diferencia  entre la lectura actual y la lectura previa para ajustar el valor máximo captado
  for (int i = 0 ; i < 5; i++) {
    if ((abs(filtered[i] - Previousfiltered[i]) >= 250)) {
      sensorMax[i] = sensorValue[i];
    }
  }

  // Declare min and max running values
  for (int i = 0 ; i < 5; i++) {
    running_min[i] = min(sensorMin[i], sensorValue[i]);
    running_max[i] = max(sensorMax[i], sensorValue[i]);
  }

  //Map - normalize the captured values
  for (int i = 0 ; i < 5; i++) {
    Value[i] = mapfloat(sensorValue[i], sensorMin[i], sensorMax[i], 0, 1);
  }

  ///*
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
  delay(10);                             // arbitrary delay to limit data to serial port
}
