#include <Adafruit_NeoPixel.h>

#define NUMPIXELS 16
#define PIN_LEFT 6
#define PIN_RIGHT 7
#define BUTTON_MODE 2
#define BUTTON_UP 3
#define BUTTON_DOWN 4

// LDR & Relay
#define RELAY_PIN 5
#define LDR_PIN A0
int ldrValue = 0;
int threshold = 200;  // ambang gelap-terang

Adafruit_NeoPixel stripLeft = Adafruit_NeoPixel(NUMPIXELS, PIN_LEFT, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripRight = Adafruit_NeoPixel(NUMPIXELS, PIN_RIGHT, NEO_GRB + NEO_KHZ800);

int mode = 1;
int brightness = 128;
int rainbowSpeed = 10;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
bool firstBoot = true;

void setup() {
  stripLeft.begin();
  stripRight.begin();

  pinMode(BUTTON_MODE, INPUT_PULLUP);
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Relay OFF awal

  Serial.begin(9600);

  // Fade in warm white saat nyala pertama
  if (firstBoot) {
    for (int b = 0; b <= brightness; b += 5) {
      stripLeft.setBrightness(b);
      stripRight.setBrightness(b);
      setColorBoth(255, 100, 20);
      delay(30);
    }
    firstBoot = false;
  }

  stripLeft.setBrightness(brightness);
  stripRight.setBrightness(brightness);
  stripLeft.show();
  stripRight.show();
}

void loop() {
  handleButtons();

  switch (mode) {
    case 1: setColorBoth(255, 100, 20); break;       // Warm White
    case 2: setColorBoth(200, 200, 255); break;      // Cool White
    case 3: setColorBoth(255, 0, 0); break;          // Merah
    case 4: setColorBoth(255, 80, 0); break;         // Oranye
    case 5: setColorBoth(255, 255, 0); break;        // Kuning
    case 6: setColorBoth(0, 255, 0); break;          // Hijau
    case 7: setColorBoth(50, 200, 255); break;       // Sky Blue
    case 8: setColorBoth(0, 0, 255); break;          // Biru
    case 9: setColorBoth(128, 0, 255); break;        // Ungu
    case 10: setColorBoth(255, 0, 100); break;       // Pink
    case 11: fadeRGB(5); break;                      // Fade RGB
    case 12: rainbowBoth(rainbowSpeed); break;       // Rainbow Smooth
  }

  // ------------------------ Baca LDR dan Kontrol Relay ------------------------
  ldrValue = analogRead(LDR_PIN);
  Serial.print("LDR: ");
  Serial.println(ldrValue);

  if (ldrValue < threshold) {
    digitalWrite(RELAY_PIN, LOW);  // Relay ON (aktif saat gelap)
  } else {
    digitalWrite(RELAY_PIN, HIGH); // Relay OFF
  }

  delay(10);
}

// ------------------------ Tombol ------------------------
void handleButtons() {
  static int lastModeState = HIGH, lastUpState = HIGH, lastDownState = HIGH;

  int modeState = digitalRead(BUTTON_MODE);
  int upState = digitalRead(BUTTON_UP);
  int downState = digitalRead(BUTTON_DOWN);

  if (modeState == LOW && lastModeState == HIGH && millis() - lastDebounceTime > debounceDelay) {
    mode++;
    if (mode > 12) mode = 1;
    lastDebounceTime = millis();
  }
  lastModeState = modeState;

  if (upState == LOW && lastUpState == HIGH && millis() - lastDebounceTime > debounceDelay) {
    if (mode == 12) {
      rainbowSpeed -= 2;
      if (rainbowSpeed < 1) rainbowSpeed = 1;
    } else {
      brightness += 20;
      if (brightness > 255) brightness = 255;
      updateBrightness();
    }
    lastDebounceTime = millis();
  }
  lastUpState = upState;

  if (downState == LOW && lastDownState == HIGH && millis() - lastDebounceTime > debounceDelay) {
    if (mode == 12) {
      rainbowSpeed += 2;
      if (rainbowSpeed > 50) rainbowSpeed = 50;
    } else {
      brightness -= 20;
      if (brightness < 0) brightness = 0;
      updateBrightness();
    }
    lastDebounceTime = millis();
  }
  lastDownState = downState;
}

void updateBrightness() {
  stripLeft.setBrightness(brightness);
  stripRight.setBrightness(brightness);
}

// ------------------------ Warna Tetap ------------------------
void setColorBoth(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < NUMPIXELS; i++) {
    stripLeft.setPixelColor(i, r, g, b);
    stripRight.setPixelColor(i, r, g, b);
  }
  stripLeft.show();
  stripRight.show();
}

// ------------------------ Fade RGB ------------------------
void fadeRGB(uint8_t wait) {
  for (int j = 0; j < 256; j++) {
    uint32_t color = Wheel(j);
    for (int i = 0; i < NUMPIXELS; i++) {
      stripLeft.setPixelColor(i, color);
      stripRight.setPixelColor(i, color);
    }
    stripLeft.show();
    stripRight.show();
    delay(wait);
  }
}

// ------------------------ Rainbow Smooth ------------------------
void rainbowBoth(uint8_t wait) {
  static uint16_t j = 0;
  for (int i = 0; i < NUMPIXELS; i++) {
    int pixelIndex = (i * 256 / NUMPIXELS + j) & 255;
    stripLeft.setPixelColor(i, Wheel(pixelIndex));
    stripRight.setPixelColor(i, Wheel(pixelIndex));
  }
  stripLeft.show();
  stripRight.show();
  delay(wait);
  j++;
}

// ------------------------ Rainbow Generator ------------------------
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) return stripLeft.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  if (WheelPos < 170) {
    WheelPos -= 85;
    return stripLeft.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return stripLeft.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
