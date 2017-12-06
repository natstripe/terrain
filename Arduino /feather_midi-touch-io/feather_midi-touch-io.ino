
// Code based on Adafruit IO Analog Out Example
// Tutorial Link: https://learn.adafruit.com/adafruit-io-basics-analog-output
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Copyright (c) 2016 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"

// Solder closed jumper on bottom!

// set up the ADAFRUIT feed - name must correspond to an active feed
AdafruitIO_Feed *midi = io.feed("midi");


// See http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf Pg 31
#define VS1053_BANK_DEFAULT 0x00
#define VS1053_BANK_DRUMS1 0x78
#define VS1053_BANK_DRUMS2 0x7F
#define VS1053_BANK_MELODY 0x79

#define MIDI_NOTE_ON  0x90
#define MIDI_NOTE_OFF 0x80
#define MIDI_CHAN_MSG 0xB0
#define MIDI_CHAN_BANK 0x00
#define MIDI_CHAN_VOLUME 0x07
#define MIDI_CHAN_PROGRAM 0xC0


#if defined(__AVR_ATmega32U4__) || defined(ARDUINO_SAMD_FEATHER_M0) || defined(TEENSYDUINO) || defined(ARDUINO_STM32_FEATHER)
#define VS1053_MIDI Serial1
#elif defined(ESP32)
HardwareSerial Serial1(2);
#define VS1053_MIDI Serial1
#elif defined(ESP8266)
#define VS1053_MIDI Serial
#endif


int sensor;
//valores cercanos a los valores reales - usando ESP32 built in touchRead
//con un cuadrado de aluminio - pendiente de valores reales de la planta 

int sensorMax = 63;
int sensorMin = 14;


int note;
int vol;

int filter;
int prev_filter;

boolean midi_bank = true; //TRUE FOR MELODY - FALSE FOR PERCUSSION


// See http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf Pg 32 for more!
int VS1053_GM1_INSTRUMENT = 54;

void setup() {
  delay(10);

  Serial.begin(115200);

  // wait for serial monitor to open
  while (! Serial);

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // set up a message handler for the 'midi' feed.
  // the handleMessage function (defined below)
  // will be called whenever a message is
  // received from adafruit io.
  midi->onMessage(handleMessage);

  // wait for a connection
  while (io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

  Serial.println("VS1053 MIDI test");

  VS1053_MIDI.begin(31250); // MIDI uses a 'strange baud rate'


  //////THESE VARIABLES WILL BE MOVED TO LOOP SO THEY CAN RESPOND TO THE FEED
  /*
    //midiSetChannelBank(0, VS1053_BANK_MELODY);
    midiSetChannelBank(0, VS1053_BANK_DRUMS1);

    midiSetChannelVolume(0, 127);

    midiSetInstrument(0, VS1053_GM1_HARMONICA); */

}

void loop() {

  // io.run(); is required for all sketches.
  // it should always be present at the top of your loop
  // function. it keeps the client connected to
  // io.adafruit.com, and processes any incoming data.
  io.run();

  sensor = touchRead(T0); //A5


  /*12 Sept/ 2017 : Utilizando el banco de notas MELODY - la nota en si
     da una escala, do - re - mi - fa - sol - la - si... Sin embargo
     utilizando el banco de DRUMS1 (o 2, no importa) cada nota representa
     de hecho un instrumento. Hay que lograr modificar el BEND - Buscar.
     También modificar el delay que en este código representa el tempo de
     la composición. Debe funcionar como un intervalo.
  */

  filter = (filter * 0.95) + (sensor * 0.5);



  if (midi_bank == true) {
    midiSetChannelBank(0, VS1053_BANK_MELODY);
    note = map(sensor, sensorMin, sensorMax, 127, 0);

  } else if ( midi_bank == false) {
    // FOR USING WITH PERCUSSION
    midiSetChannelBank(0, VS1053_BANK_DRUMS1);
    note = map(sensor, 30, 33, 127, 0);
  }


  midiSetInstrument(0, VS1053_GM1_INSTRUMENT);


  //
  note = map(sensor, sensorMin, sensorMax, 127, 0);
  vol = map(sensor, sensorMin, sensorMax, 127, 50);

  //
  note = constrain(note, 0, 127);
  vol = constrain(vol, 0, 127);



  midiNoteOn(0, note, 127);
  delay(filter/10);
  midiNoteOff(0, note, 127);

  /* for (uint8_t i=60; i<69; i++) {
     midiNoteOn(0, i, 127);
     delay(100);
     midiNoteOff(0, i, 127);
    }*/
  /*
    Serial.print("FilterdValue=");
    Serial.print(filter);
    Serial.print("\t");
    Serial.print("SensorValue=");
    Serial.println(sensor);
    Serial.print("\n");
  */

  //delay(100);
}




// this function is called whenever a 'midi' message
// is received from Adafruit IO. it was attached to
// the midi feed in the setup() function above.
void handleMessage(AdafruitIO_Data *data) {

  // convert the data to integer
  int reading = data->toInt();

  Serial.print("received <- ");
  Serial.println(reading);
  VS1053_GM1_INSTRUMENT = reading;
  midiSetInstrument(0, VS1053_GM1_INSTRUMENT);

}






void midiSetInstrument(uint8_t chan, uint8_t inst) {
  if (chan > 15) return;
  inst --; // page 32 has instruments starting with 1 not 0 :(
  if (inst > 127) return;

  VS1053_MIDI.write(MIDI_CHAN_PROGRAM | chan);
  delay(10);
  VS1053_MIDI.write(inst);
  delay(10);
}


void midiSetChannelVolume(uint8_t chan, uint8_t vol) {
  if (chan > 15) return;
  if (vol > 127) return;

  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write(MIDI_CHAN_VOLUME);
  VS1053_MIDI.write(vol);
}

void midiSetChannelBank(uint8_t chan, uint8_t bank) {
  if (chan > 15) return;
  if (bank > 127) return;

  VS1053_MIDI.write(MIDI_CHAN_MSG | chan);
  VS1053_MIDI.write((uint8_t)MIDI_CHAN_BANK);
  VS1053_MIDI.write(bank);
}

void midiNoteOn(uint8_t chan, uint8_t n, uint8_t vel) {
  if (chan > 15) return;
  if (n > 127) return;
  if (vel > 127) return;

  VS1053_MIDI.write(MIDI_NOTE_ON | chan);
  VS1053_MIDI.write(n);
  VS1053_MIDI.write(vel);
}

void midiNoteOff(uint8_t chan, uint8_t n, uint8_t vel) {
  if (chan > 15) return;
  if (n > 127) return;
  if (vel > 127) return;

  VS1053_MIDI.write(MIDI_NOTE_OFF | chan);
  VS1053_MIDI.write(n);
  VS1053_MIDI.write(vel);
}
