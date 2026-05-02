#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino_BMI270_BMM150.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Buttons
#define BTN_NEXT 2
#define BTN_PREV 3
#define BTN_ACTION 4

int page = 0;
int actionState = 0;

// IMU values
float ax, ay, az;
float shakeLevel = 0;

void drawPage();

void setup() {
  pinMode(BTN_NEXT, INPUT_PULLUP);
  pinMode(BTN_PREV, INPUT_PULLUP);
  pinMode(BTN_ACTION, INPUT_PULLUP);

  Serial.begin(115200);

  if (!IMU.begin()) {
    Serial.println("IMU failed!");
    while (1);
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (1);
  }

  drawPage();
}

void loop() {

  // -------- BUTTON LOGIC --------
  if (digitalRead(BTN_NEXT) == LOW) {
    page = (page + 1) % 3;
    drawPage();
    delay(200);
  }

  if (digitalRead(BTN_PREV) == LOW) {
    page = (page - 1 + 3) % 3;
    drawPage();
    delay(200);
  }

  if (digitalRead(BTN_ACTION) == LOW) {
    actionState++;
    drawPage();
    delay(200);
  }

  // -------- IMU READ --------
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(ax, ay, az);

    // Shake detection (simple magnitude change)
    float mag = abs(ax) + abs(ay) + abs(az);
    shakeLevel = mag;
  }

  // refresh motion pages smoothly
  if (page != 0) {
    drawPage();
    delay(50);
  }
}

// ---------------- DRAW ----------------
void drawPage() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // ===== PAGE 0: MENU =====
  if (page == 0) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("PAGE 0: MENU");

    display.print("Action: ");
    display.println(actionState);

    display.println("BTN NEXT/PREV to switch");
  }

  // ===== PAGE 1: TILT BALL =====
  else if (page == 1) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("PAGE 1: TILT BALL");

    // map tilt to screen position
    int x = map(ax * 100, -100, 100, 0, SCREEN_WIDTH);
    int y = map(ay * 100, -100, 100, 0, SCREEN_HEIGHT);

    // clamp
    x = constrain(x, 0, SCREEN_WIDTH - 5);
    y = constrain(y, 10, SCREEN_HEIGHT - 5);

    display.fillCircle(x, y, 4, SSD1306_WHITE);

    display.setCursor(0, 54);
    display.print("ax:");
    display.print(ax, 2);
    display.print(" ay:");
    display.print(ay, 2);
  }

  // ===== PAGE 2: SHAKE DETECTOR =====
  else if (page == 2) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("PAGE 2: SHAKE TEST");

    display.setCursor(0, 20);
    display.print("Shake level:");
    display.println(shakeLevel, 2);

    // visual bar
    int bar = map(shakeLevel * 10, 10, 40, 0, 120);
    bar = constrain(bar, 0, 120);

    display.drawRect(0, 40, 120, 10, SSD1306_WHITE);
    display.fillRect(0, 40, bar, 10, SSD1306_WHITE);

    if (shakeLevel > 2.5) {
      display.setCursor(0, 54);
      display.println("SHAKE DETECTED!");
    }
  }

  display.display();
}