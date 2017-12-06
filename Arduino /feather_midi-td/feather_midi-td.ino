// Blinking function written by FabrizioP @ Instructables.com</p>


// Solder closed jumper on bottom!

// See http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf Pg 31
#define VS1053_BANK_DEFAULT 0x00
#define VS1053_BANK_DRUMS1 0x78
#define VS1053_BANK_DRUMS2 0x7F
#define VS1053_BANK_MELODY 0x79

// See http://www.vlsi.fi/fileadmin/datasheets/vs1053.pdf Pg 32 for more!
//#define VS1053_GM1_INSTRUMENT 63

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
int sensorMax = 60;
int sensorMin = 15;


int note;
int vol;

int filter;
int min_filtered;
int max_filtered;

boolean midi_bank = true;

int x; //incoming serial

int VS1053_GM1_INSTRUMENT = 53;
//54 WE LIKE ! - ACCORDING TO MIDI SHEET 54 Voice Oohs 



void setup() {
  delay(100);

  Serial.begin(115200);

  Serial.println("VS1053 MIDI test");

  VS1053_MIDI.begin(31250); // MIDI uses a 'strange baud rate'

  midiSetChannelVolume(0, 127);

  Serial.println("Starting calibration for 5 seconds");
  // calibrate during the first five seconds
  while (millis() < 5000)
  {
    sensor = touchRead(T0);    // read sensor connected to analog pin 0

    if (sensor > sensorMax)    // save the maximum sensor value found
    {
      sensorMax = sensor;
    }

    if (sensor < sensorMin)    // save the minimum sensor value found
    {
      sensorMin = sensor;
    }
  }
  Serial.println("Finished calibration");

  Serial.println("Print results");

  while (millis() < 10000)
  {
    Serial.print("Minimo");
    Serial.print(" ");
    Serial.print(sensorMin);
    Serial.print(" ");

    Serial.print("Maximo");
    Serial.print(" ");
    Serial.println(sensorMax);

  }


}

void loop() {


  sensor = touchRead(T0); //A5

  /*12 Sept/ 2017 : Utilizando el banco de notas MELODY - la nota en si da una escala, do - re - mi - fa - sol - la - si... Sin embargo
     utilizando el banco de DRUMS1 (o 2, no importa) cada nota representa de hecho un instrumento. Hay que lograr modificar el
     BEND - Buscar . También modificar el delay que en este código representa el tempo de la composición. Debe funcionar
     como un intervalo.
  */

  filter = (filter * 0.95) + (sensor * 0.5);

  min_filtered = (min_filtered * 0.95) + (sensorMin * 0.5);

  max_filtered = (max_filtered * 0.95) + (sensorMax * 0.5);


  if (midi_bank == true) {
    midiSetChannelBank(0, VS1053_BANK_MELODY);
    note = map(sensor, sensorMin, sensorMax, 127, 0);

  } else if ( midi_bank == false) {
    // FOR USING WITH PERCUSSION
    midiSetChannelBank(0, VS1053_BANK_DRUMS1);
    note = map(sensor, 30, 33, 127, 0);
  }


  midiSetInstrument(0, VS1053_GM1_INSTRUMENT);


  vol = map(sensor, sensorMin, sensorMax, 127, 50);

  //
  note = constrain(note, 0, 127);
  vol = constrain(vol, 50, 127);



  midiNoteOn(0, note, 127);
  delay(filter / 10);
  midiNoteOff(0, note, 127);

  /* for (uint8_t i=60; i<69; i++) {
     midiNoteOn(0, i, 127);
     delay(100);
     midiNoteOff(0, i, 127);
    }*/


  Serial.print("SensorMin=");
  Serial.print(sensorMin);
  Serial.print("\t");
  Serial.print("sensorMax=");
  Serial.print(sensorMax);
  Serial.print("\t");

  Serial.print("FilteredMin=");
  Serial.print(min_filtered);
  Serial.print("\t");
  Serial.print("FilteredMax=");
  Serial.print(max_filtered);
  Serial.print("\t");


  Serial.print("FilteredValue=");
  Serial.print(filter);
  Serial.print("\t");
  Serial.print("SensorValue=");
  Serial.println(sensor);
  Serial.print("\n");


  delay(100);
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
