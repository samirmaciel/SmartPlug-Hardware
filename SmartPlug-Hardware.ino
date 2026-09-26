#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ZMPT101B.h>

// ==================================================
// Display
// ==================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET -1
#define OLED_ADDRESS 0x3C

// ==================================================
// Hardware pins
// ==================================================

#define RELAY_PIN 7
#define VOLTAGE_SENSOR_PIN A2

#define LEFT_BUTTON_PIN 6
#define MIDDLE_BUTTON_PIN 5
#define RIGHT_BUTTON_PIN 4

// ==================================================
// Screens
// ==================================================

#define HOME_SCREEN 0
#define SETTINGS_SCREEN 1
#define SET_TIMER_SCREEN 2

// ==================================================
// Voltage sensor
// ==================================================

#define VOLTAGE_SENSITIVITY 500.0f

// ==================================================
// Display
// ==================================================

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

// ==================================================
// Application state
// ==================================================

ZMPT101B voltageSensor(VOLTAGE_SENSOR_PIN, 60.0);

int powerOutageCount = 0;

float voltage = 0.0;

bool powerGridConnected = false;
bool outletPowered = false;
bool automaticMode = true;

bool lastRightButtonState = HIGH;
bool lastMiddleButtonState = HIGH;
bool lastLeftButtonState = HIGH;

const float powerGridThreshold = 70.0;

unsigned long remainingTime = 0;
unsigned long selectedTime = 30;

unsigned long lastTimerUpdate = 0;
unsigned long lastVoltageReadTime = 0;

unsigned long voltageReadInterval = 2000;

unsigned int currentScreen = HOME_SCREEN;

// ==================================================
// Icons
// ==================================================

const unsigned char OPTION_ITEM_1[] PROGMEM = {
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0x8f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0x90, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0xa2, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0xa2, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0xc2, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0xc2, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0xc3, 0xd0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0xc0, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0xa0, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0xa0, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0x90, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0x8f, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe,
};

const unsigned char ICON_OPTIONS[] PROGMEM = {
  0x00, 0x00, 
  0x00, 0x00, 
  0x00, 0x00, 
  0x00, 0x00, 
  0x00, 0x00, 
  0x00, 0x00, 
  0xff, 0xff, 
  0xff, 0xff, 
  0x00, 0x00, 
  0xff, 0xff, 
  0xff, 0xff, 
  0x00, 0x00, 
  0xff, 0xff, 
  0xff, 0xff, 
  0x00, 0x00, 
  0x00, 0x00,
};

const unsigned char ICON_ARROW_LEFT[] PROGMEM = {
  0x00, 0x00, 
  0x00, 0x00, 
  0x00, 0x00, 
  0x00, 0x00, 
  0x00, 0x00, 
  0x00, 0x80, 
  0x01, 0x80, 
  0x03, 0x80, 
  0x07, 0x80, 
  0x0f, 0x80, 
  0x07, 0x80, 
  0x03, 0x80, 
  0x01, 0x80, 
  0x00, 0x80, 
  0x00, 0x00, 
  0x00, 0x00,
};

const unsigned char ICON_SELECT[] PROGMEM = {
  0x00, 0x00, 
  0x00, 0x00, 
  0x00, 0x00, 
  0x00, 0x00, 
  0x00, 0x00, 
  0x00, 0x00, 
  0xff, 0xff, 
  0xff, 0xff, 
  0x80, 0x01, 
  0xbf, 0xfd, 
  0xbf, 0xfd, 
  0x80, 0x01, 
  0xff, 0xff, 
  0xff, 0xff, 
  0x00, 0x00, 
  0x00, 0x00,
};

const unsigned char ICON_CHANGE[] PROGMEM = {
  0x00, 0x00, 
  0x00, 0x00, 
  0x00, 0x00, 
  0x00, 0x00, 
  0x00, 0x00, 
  0x01, 0x80, 
  0x03, 0xc0, 
  0x07, 0xe0, 
  0x0f, 0xf0, 
  0x1f, 0xf8, 
  0x00, 0x00, 
  0x1f, 0xf8, 
  0x1f, 0xf8, 
  0x00, 0x00, 
  0x00, 0x00, 
  0x00, 0x00, 
};

const unsigned char ICON_SETTINGS[] PROGMEM = {
  0x00, 0x00,
  0x01, 0xc0,
  0x03, 0xc0,
  0x03, 0xc0,
  0x3f, 0xfc,
  0x3f, 0xfc,
  0x3c, 0x7c,
  0x1c, 0x38,
  0x1c, 0x38,
  0x3e, 0x7c,
  0x3f, 0xfc,
  0x3f, 0xfc,
  0x03, 0xc0,
  0x03, 0xc0,
  0x03, 0xc0,
  0x00, 0x00,
};

const unsigned char SETTINGS_SCREEN_BOTTOM[] PROGMEM = {
  0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 
  0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 
  0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 
  0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4f, 0xf9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xe0, 
  0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4f, 0xf9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf0, 
  0x0f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4f, 0xf9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xf8, 
  0x1f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4f, 0xf9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfc, 
  0x3f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4f, 0xf9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x1f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4f, 0xf9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xfc, 
  0x0f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4f, 0xf9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xf8, 
  0x07, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4f, 0xf9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xf0, 
  0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4f, 0xf9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xe0, 
  0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xc0, 
  0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 
  0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 
};

// ==================================================
// Drawing functions
// ==================================================

void drawPlug(int x, int y) {

  // Outlet body
  display.drawRoundRect(x + 2,y + 6, 14, 16, 2, SSD1306_WHITE);

  // Pins
  display.drawLine(x + 5, y, x + 5, y + 6, SSD1306_WHITE);

  display.drawLine(
    x + 13,
    y,
    x + 13,
    y + 6,
    SSD1306_WHITE
  );

  // Cable
  display.drawLine(
    x + 9,
    y + 22,
    x + 9,
    y + 27,
    SSD1306_WHITE
  );
}

void drawClock(int x, int y) {

  display.drawCircle(
    x + 8,
    y + 8,
    7,
    SSD1306_WHITE
  );

  display.drawLine(
    x + 8,
    y + 8,
    x + 8,
    y + 3,
    SSD1306_WHITE
  );

  display.drawLine(
    x + 8,
    y + 8,
    x + 11,
    y + 10,
    SSD1306_WHITE
  );
}

void drawGear(int x, int y) {

  display.drawCircle(
    x + 8,
    y + 8,
    7,
    SSD1306_WHITE
  );

  display.fillCircle(
    x + 8,
    y + 8,
    3,
    SSD1306_BLACK
  );

  // Simplified teeth
  display.drawLine(
    x + 8,
    y,
    x + 8,
    y + 2,
    SSD1306_WHITE
  );

  display.drawLine(
    x + 8,
    y + 14,
    x + 8,
    y + 16,
    SSD1306_WHITE
  );

  display.drawLine(
    x,
    y + 8,
    x + 2,
    y + 8,
    SSD1306_WHITE
  );

  display.drawLine(
    x + 14,
    y + 8,
    x + 16,
    y + 8,
    SSD1306_WHITE
  );
}

// ==================================================
// Countdown
// ==================================================

void drawCountdown() {

  int hours = remainingTime / 3600;
  int minutes = (remainingTime % 3600) / 60;
  int seconds = remainingTime % 60;

  char timer[9];

  sprintf(
    timer,
    "%02d:%02d:%02d",
    hours,
    minutes,
    seconds
  );

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  // Approximately centered
  display.setCursor(24, 28);

  display.print(timer);
}

// ==================================================
// Screen drawing
// ==================================================

void drawHomeScreen() {

  currentScreen = HOME_SCREEN;

  display.clearDisplay();

  // ==================================================
  // Top row
  // ==================================================

  // Power outages
  display.setTextSize(1);
  display.setCursor(2, 1);
  display.print("QUEDAS:");

  display.setCursor(45, 1);
  display.print(powerOutageCount);

  // Divider
  display.drawLine(
    66,
    0,
    66,
    13,
    SSD1306_WHITE
  );

  // Voltage
  display.setCursor(85, 1);
  display.print("v");

  display.setCursor(95, 1);
  display.print(voltage, 1);

  // ==================================================
  // Divider
  // ==================================================

  display.drawLine(
    0,
    13,
    127,
    13,
    SSD1306_WHITE
  );

  // ==================================================
  // Center area
  // ==================================================

  if (automaticMode && !outletPowered && remainingTime > 0) {
    drawClock(2, 27);
    display.setCursor(24, 18);
    display.print("RELIGANDO EM");
    drawCountdown();

  } else {
    drawPlug(5, 18);
    display.setTextSize(1);
    display.setCursor(27, 23);
    if (outletPowered) {
      display.print("TOMADA");
      display.setCursor(27, 32);
      display.print("LIGADA");
    } else {
      display.print("TOMADA");
      display.setCursor(27, 32);
      display.print("DESLIGADA");
    }
  }

  // ==================================================
  // Bottom area
  // ==================================================

  display.drawLine(
    0,
    48,
    126,
    48,
    SSD1306_WHITE
  );

  // AUTO / MANUAL
  display.setTextSize(1);

  if (automaticMode) {
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(4, 55);
    display.print("AUTO");
  } else {
    display.setCursor(2, 55);
    display.print("MANUAL");
  }

  // Settings
  display.drawBitmap(60, 48, ICON_OPTIONS, 16, 16, SSD1306_WHITE);

  if (outletPowered) {
    display.setCursor(106, 55);
    display.print("ON");
  } else {
    display.setCursor(106, 55);
    display.print("OFF");
  }
  display.display();
}

void drawSettingsScreen() {
  currentScreen = SETTINGS_SCREEN;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 4);
  display.print("Tempo para religar");
  display.setCursor(15, 14);
  display.print("00:00:30");
  
  display.drawBitmap(0, 0, OPTION_ITEM_1, 128, 23, SSD1306_WHITE);
  display.fillRect(0,22, 126, 1, SSD1306_WHITE);
  display.drawLine(0, 48, 128, 48, SSD1306_WHITE);
  display.drawBitmap(60, 48, ICON_SELECT, 16, 16, SSD1306_WHITE);
  display.drawBitmap(0, 48, ICON_ARROW_LEFT, 16, 16, SSD1306_WHITE);
  display.drawBitmap(112, 48, ICON_CHANGE, 16, 16, SSD1306_WHITE);
  display.display();
}

void drawSetTimerScreen() {

  currentScreen = SET_TIMER_SCREEN;
}

// ==================================================
// Voltage
// ==================================================

float readVoltage() {
  float voltageValue = voltageSensor.getRmsVoltage();
  voltage = 0.0;
  
  if (voltageValue > 20.0) {
    voltage = voltageValue;
    return voltageValue;
  }

  return voltage;
}

// ==================================================
// Button handling
// ==================================================

void readRightButton() {

  bool currentRightButtonState = digitalRead(RIGHT_BUTTON_PIN);

  if (
    lastRightButtonState == LOW &&
    currentRightButtonState == HIGH
  ) {

    if (currentScreen == HOME_SCREEN) {
      outletPowered = !outletPowered;
      if (automaticMode) {
        automaticMode = false;
      }

    } else if (currentScreen == SETTINGS_SCREEN) {
      // TODO change selected item in menu
    } else {
      // TODO Increase timer
    }
  }

  lastRightButtonState = currentRightButtonState;
}

void readMiddleButton() {

  bool currentMiddleButtonState = digitalRead(MIDDLE_BUTTON_PIN);

  if (
    lastMiddleButtonState == LOW &&
    currentMiddleButtonState == HIGH
  ) {

    if (currentScreen == HOME_SCREEN) {
      drawSettingsScreen();
    } else if (currentScreen == SETTINGS_SCREEN) {
      drawSetTimerScreen();
    } else {
      drawSettingsScreen();
    }
  }

  lastMiddleButtonState = currentMiddleButtonState;
}

void readLeftButton() {

  bool currentLeftButtonState = digitalRead(LEFT_BUTTON_PIN);

  if (lastLeftButtonState == LOW && currentLeftButtonState == HIGH ) {

    if (currentScreen == HOME_SCREEN) {
      automaticMode = !automaticMode;
    } else if (currentScreen == SETTINGS_SCREEN) {
      drawHomeScreen();
    } else {
      // TODO Change position in Timer selection
    }
  }

  lastLeftButtonState = currentLeftButtonState;
}

void readButtons() {

  readLeftButton();
  readMiddleButton();
  readRightButton();
}

// ==================================================
// Screen logic
// ==================================================

void runTimerScreenLogic() {

}

void runSettingsScreenLogic() {

}

void runHomeScreenLogic() {

  if (millis() - lastVoltageReadTime > voltageReadInterval) {
    lastVoltageReadTime = millis();
    readVoltage();
  }

  if (!automaticMode) {
    remainingTime = selectedTime;
  }

  if (outletPowered) {
    digitalWrite(RELAY_PIN,LOW);
  } else {
    digitalWrite(RELAY_PIN,HIGH);
  }

  if (automaticMode && remainingTime > 0) {
    if (millis() - lastTimerUpdate >= 1000) {
      lastTimerUpdate = millis();
      remainingTime--;
      drawHomeScreen();
    }

  } else {
    if (automaticMode == true && outletPowered == false && readVoltage() > 90) {
      outletPowered = true;
    } else if ( automaticMode == true && outletPowered == false) {
      remainingTime = selectedTime;
    }
    drawHomeScreen();
  }

  if (voltage < 90 && outletPowered == true && automaticMode == true && selectedTime > 0) {
    outletPowered = false;
    remainingTime = selectedTime;
    drawHomeScreen();
  }
}

// ==================================================
// Setup
// ==================================================

void setup() {

  Serial.begin(9600);

  pinMode(RELAY_PIN,OUTPUT);
  pinMode(VOLTAGE_SENSOR_PIN,INPUT);
  pinMode(LEFT_BUTTON_PIN,INPUT_PULLUP);
  pinMode(MIDDLE_BUTTON_PIN,INPUT_PULLUP);
  pinMode(RIGHT_BUTTON_PIN,INPUT_PULLUP);

  voltageSensor.setSensitivity(VOLTAGE_SENSITIVITY);

  if (!display.begin(SSD1306_SWITCHCAPVCC,OLED_ADDRESS)) {
    Serial.println("Oled failed");
    while (true);
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  drawHomeScreen();
}

// ==================================================
// Main loop
// ==================================================

void loop() {
  readButtons();

  if (currentScreen == HOME_SCREEN) {
    runHomeScreenLogic();
  } else if (currentScreen == SETTINGS_SCREEN) {
    runSettingsScreenLogic();
  } else {
    runTimerScreenLogic();
  }
}
