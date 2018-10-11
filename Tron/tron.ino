#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define OUTER_PIN 0
#define INNER_PIN 1
#define OUTER_RING_PIXELS 37
#define INNER_RING_PIXELS 25

Adafruit_NeoPixel outerRing = Adafruit_NeoPixel(OUTER_RING_PIXELS, OUTER_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel innerRing = Adafruit_NeoPixel(INNER_RING_PIXELS, INNER_PIN, NEO_GRB + NEO_KHZ800);
int delayval = 5;

uint32_t color;
int index = 0;
bool loaded;

void setup() {
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  innerRing.setBrightness(50);
  
  innerRing.begin();
  outerRing.begin(); // This initializes the NeoPixel library.
  innerRing.show();
  outerRing.show();

  color = outerRing.Color(0, 190, 190);
  index = 0;
  loaded = false;
}

void loop() {
  if(!loaded) {
    for(int i = 0; i < INNER_RING_PIXELS; ++i) {
      innerRing.setPixelColor(i, color);
      innerRing.show();
      delay(120);
    }
    delay(120);
    for(int i = 0; i < OUTER_RING_PIXELS; ++i) {
      outerRing.setPixelColor(i, color);
    }
    outerRing.show();
    delay(750);
    loaded = true;
  }
  else {
    int half = (OUTER_RING_PIXELS / 2.0f) + 0.5f;
    for(int i=0; i<OUTER_RING_PIXELS; i++) {
      outerRing.setPixelColor(i, outerRing.Color(0, 0, 0));
    }
    outerRing.show();
  
    outerRing.setPixelColor(index, color);
    outerRing.setPixelColor(index + half, color);
    outerRing.show();
    delay(delayval);
  
    ++index %= half;
  }
}
