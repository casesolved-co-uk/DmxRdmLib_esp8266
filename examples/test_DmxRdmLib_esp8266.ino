/*
Test DmxRdmLib_esp8266 dual channel output
Tested on PAR with Chan 1-3: RGB
*/

#include <DmxRdmLib.h>


#define TRIGGER 13    // Used for oscilloscope
#define DMX_DIR_A 5
#define DMX_DIR_B 16
#define NUM_CHANS 512 // Must be 512
#define MIN_CHANS 15  // Controls DMX frame rate
byte* dataA;
byte* dataB;

// Convert hue to RGB assuming 0.5 Luminance and 1.0 Saturation
// Simplified https://stackoverflow.com/questions/46169415/understanding-hsl-to-rgb-color-space-conversion-algorithm
// v1 = 0.0, v2 = 1.0
void huetoRGB (const uint8_t hue, uint8_t *RGB) {
  RGB[0] = hueconv(hue + 85);   // R
  RGB[1] = hueconv(hue);        // G
  RGB[2] = hueconv(hue - 85);   // B
}

// 0 <= vH <= 255
uint8_t hueconv(int vH) {
  if (vH < 0) vH += 256;
  if (vH > 255) vH -= 256;
  if (vH <= 42) return 6 * vH;
  if (vH <= 127) return 255;
  if (vH <= 170) return (170 - vH) * 6;
  return 0;
}

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

  // dmx driver finds high watermark:
  // setChans does a data diff, chanUpdate looks for non-zeros
  dataA[60] = 0xFF;
  dataB[100] = 0x55;

  // To disable just don't call begin
  // void begin(uint8_t dir, byte* buf, uint16_t min_chans, bool invert);
  dmxA.begin(DMX_DIR_A, dataA, MIN_CHANS, true);
  dmxB.begin(DMX_DIR_B, dataB, MIN_CHANS, true);

  // Don't update chan B - should go to idle sending a full DMX frame every 800ms
  // Full frame size unaffected by MIN_CHANS
  dmxB.chanUpdate(MIN_CHANS);
}

uint8_t hue = 0;
uint32_t loopcount = 0;
void loop()
{
  // Make less flashy with a delay loopcount
  if (loopcount++ % 20 == 0) {
    // must change data (only using setChans) otherwise nothing is sent
    huetoRGB(hue, dataA);
    memcpy(dataB, dataA, 4);
    hue = (hue+1) % 256;
  }

  // must call setChans or chanUpdate for each new data
  dmxA.chanUpdate(MIN_CHANS);

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
