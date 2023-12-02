/*
Test DmxRdmLib_esp8266 dual channel output
*/

#include <DmxRdmLib.h>


#define TRIGGER 13
#define DMX_DIR_A 5
#define DMX_DIR_B 16
#define NUM_CHANS 512
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
  dataA = (byte*)os_zalloc(sizeof(byte) * NUM_CHANS);
  dataB = (byte*)os_zalloc(sizeof(byte) * NUM_CHANS);

  // dmx driver finds high watermark by looking for non-zero data
  // setChans does a data diff, chanUpdate looks for non-zeros
  dataA[60] = 0xFF;
  dataB[100] = 0x55;

  // To disable just don't call begin
  dmxA.begin(DMX_DIR_A, dataA);
  dmxB.begin(DMX_DIR_B, dataB);

  // Don't update chan B - should go to idle sending every 800ms
  dmxB.chanUpdate(NUM_CHANS);
}

void loop()
{
  // must change data using setChans otherwise nothing is sent
  for (uint8_t i = 0; i < 10; i++) {
    dataA[i]++;
    dataB[i]++;
  }
  // must call setChans or chanUpdate for each new data
  dmxA.chanUpdate(NUM_CHANS);

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
