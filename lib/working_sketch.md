#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Button pins
#define BTN_NEXT 2
#define BTN_PREV 3
#define BTN_ACTION 4

int page = 0;
int actionState = 0;
void drawPage();

void setup() {
  pinMode(BTN_NEXT, INPUT_PULLUP);
  pinMode(BTN_PREV, INPUT_PULLUP);
  pinMode(BTN_ACTION, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    for(;;);
  }

  drawPage();
}

void loop() {
  if(digitalRead(BTN_NEXT) == LOW) {
    page++;
    if(page > 2) page = 0;
    drawPage();
    delay(200); // debounce
  }

  if(digitalRead(BTN_PREV) == LOW) {
    page--;
    if(page < 0) page = 2;
    drawPage();
    delay(200);
  }

  if(digitalRead(BTN_ACTION) == LOW) {
    actionState++;
    drawPage();
    delay(200);
  }
}

void drawPage() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  if(page == 0) {
    display.setCursor(0,0);
    display.println("Page 1: Buttons!");
    display.println("Press buttons!");

    display.setCursor(0,20);
    display.print("Action count: ");
    display.println(actionState);
  }

  else if(page == 1) {
    display.setCursor(0,0);
    display.println("Page 2: Shapes");

    display.drawRect(10, 20, 50, 30, SSD1306_WHITE);
    display.fillCircle(90, 35, 10, SSD1306_WHITE);
  }

  else if(page == 2) {
    display.setCursor(0,0);
    display.println("Page 3: Debug");

    display.setCursor(0,20);
    display.print("NEXT: ");
    display.println(digitalRead(BTN_NEXT) == LOW ? "PRESSED" : "OK");

    display.print("PREV: ");
    display.println(digitalRead(BTN_PREV) == LOW ? "PRESSED" : "OK");

    display.print("ACT: ");
    display.println(digitalRead(BTN_ACTION) == LOW ? "PRESSED" : "OK");
  }

  display.display();
}