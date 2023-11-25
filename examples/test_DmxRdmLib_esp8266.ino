/*
Test DmxRdmLib_esp8266 dual channel output
*/

#include <DmxRdmLib.h>


#define TRIGGER 13
#define DMX_DIR_A 5
#define DMX_DIR_B 16
byte* dataA;
byte* dataB;


void setup()
{
#ifdef TRIGGER
  // Low by default
  digitalWrite(TRIGGER, LOW);
  pinMode(TRIGGER, OUTPUT);
  digitalWrite(TRIGGER, LOW);
#endif

  // allocating dmx data memory block
  dataA = (byte*)os_zalloc(sizeof(byte) * 512);
  dataB = (byte*)os_zalloc(sizeof(byte) * 512);

  // must change data otherwise nothing is sent
  // dmx driver finds high watermark by looking for non-zero data
  dataA[60] = 0xFF;
  dataB[100] = 0x55;

  // To disable just don't call begin
  dmxA.begin(DMX_DIR_A);
  dmxB.begin(DMX_DIR_B);
  // must call setChans at least once to start
  dmxA.setChans(dataA, 512);
  dmxB.setChans(dataB, 512);
}

void loop()
{
  for (uint8_t i = 0; i < 10; i++) {
    dataA[i]++;
    dataB[i]++;
  }

#ifdef TRIGGER
  digitalWrite(TRIGGER, HIGH);
#endif

  dmxA.handler();
  dmxB.handler();

#ifdef TRIGGER
  digitalWrite(TRIGGER, LOW);
#endif

  delay(1);
}
