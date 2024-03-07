#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Stepper.h>

// Function prototype declaration
bool connectToServer();

// Function definition
bool connectToServer() {
  // Implementation of the function
  return true; // Example return value
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) override {
        Serial.println("Device found");
        // Additional implementation
    }
};

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Stepper motor setup
const int stepsPerRevolution = 200; // Change this to the number of steps per revolution for your motor
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11); // Pins for the stepper motor

// UUIDs for BLE service and characteristic
#define SERVICE_UUID        "53fac100-ad46-4ca6-b8bb-26cb1ebe3af6"
#define CHARACTERISTIC_UUID "6325e571-ad46-4c13-9088-6762baf82ff8"

boolean doConnect = false;
boolean connected = false;
BLERemoteCharacteristic* pRemoteCharacteristic;
BLEAdvertisedDevice* myDevice;

static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    float humidity = atof((char*)pData);
    
    // Update display with humidity
    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Humidity: ");
    display.print(humidity);
    display.println("%");
    int needleLength = map(humidity, 0, 100, 0, SCREEN_WIDTH);
    display.drawLine(0, SCREEN_HEIGHT/2, needleLength, SCREEN_HEIGHT/2, SSD1306_WHITE);
    display.display();

    // Move stepper motor based on humidity
    int steps = map(humidity, 0, 100, 0, stepsPerRevolution);
    myStepper.step(steps - stepsPerRevolution / 2); // Move stepper to position representing humidity
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    connected = true;
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Disconnected");
    display.display();
  }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init("");

  // Initialize OLED display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Loop forever
  }
  display.display();
  delay(2000); // Pause for 2 seconds
  display.clearDisplay();

  // Initialize stepper motor
  myStepper.setSpeed(60); // Set stepper speed

  // Register notification callback
  if(pRemoteCharacteristic->canNotify())
    pRemoteCharacteristic->registerForNotify(notifyCallback);

  // Start scanning for BLE servers with the service we're interested in
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}


void loop() {
  // The loop function remains largely the same...
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("Connected to the BLE Server.");
    } else {
      Serial.println("Failed to connect to the server.");
    }
    doConnect = false;
  }

  // Additional logic for re-connecting or handling connected state can be added here
  delay(1000); // Delay to ease the loop
}
