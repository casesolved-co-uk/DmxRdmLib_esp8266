/*
Since UART1 is TX only, it cannot have Input enabled, so is plain DMX out only
Enabling input pauses/disables RDM
This means we can't test RDM in loopback

For Port A input with loopback cable to Port B output
*/

#include <DmxRdmLib.h>

#define ESTA_MAN 0x08DD      // ESTA Manufacturer Code
#define ESTA_DEV 0xEE000000  // RDM Device ID (used with Man Code to make 48bit UID)

#define TRIGGER 13 // MOSI
#define DMXIN_FLAG 14 // SCLK
#define DMX_DIR_A 5
#define DMX_DIR_B 16
byte* dataA;
byte* dataB;

byte* dmxInBuf = NULL;
uint16_t dmxInRxChans = 0;


void setup()
{
  pinMode(DMXIN_FLAG, OUTPUT);
  digitalWrite(DMXIN_FLAG, LOW);
#ifdef TRIGGER
  // Low by default
  digitalWrite(TRIGGER, LOW);
  pinMode(TRIGGER, OUTPUT);
  digitalWrite(TRIGGER, LOW);
#endif

  // allocating dmx data memory blocks
  dataA = (byte*)os_zalloc(sizeof(byte) * 512);
  dataB = (byte*)os_zalloc(sizeof(byte) * 512);

  // must change data otherwise nothing is sent
  // dmx driver finds high watermark by looking for non-zero data
  dataB[60] = 0xFF;

  // DMX Input uses two buffers and switches between them
  dmxA.begin(DMX_DIR_A, dataA);
  dmxA.dmxIn(true);
  dmxA.setInputCallback(dmxIn);
  dmxInBuf = (byte*) os_zalloc(sizeof(byte) * 512);

  // To disable just don't call begin
  dmxB.begin(DMX_DIR_B);
  // must call setChans at least once to start
  dmxB.setChans(dataB, 512);
}

void loop()
{
  for (uint8_t i = 0; i < 10; i++) {
    dataB[i]++;
  }

#ifdef TRIGGER
  digitalWrite(TRIGGER, HIGH);
#endif
  // RDM handled here by rdmRXTimeout
  dmxA.handler();
#ifdef TRIGGER
  digitalWrite(TRIGGER, LOW);
#endif

#ifdef TRIGGER
  digitalWrite(TRIGGER, HIGH);
#endif
  dmxB.handler();
#ifdef TRIGGER
  digitalWrite(TRIGGER, LOW);
#endif

  // Handle received DMX - send it back out
  if (dmxInRxChans) {
    digitalWrite(DMXIN_FLAG, HIGH);
    memcpy(dataB, dmxInBuf, sizeof(byte) * dmxInRxChans);
    dmxInRxChans = 0;
    digitalWrite(DMXIN_FLAG, LOW);
  }

  delay(1);
}

// DMX callback
void dmxIn(uint16_t numChans) {
  dmxInRxChans = numChans;
  espDMX* dmx = &dmxA;

  // could be overwritten with 0
  byte* tmp = dmxInBuf;
  dmxInBuf = dmx->getChans();
  // can be called with NULL to allocate a new buffer
  dmx->setBuffer(tmp);
}
