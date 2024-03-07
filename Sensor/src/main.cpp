#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <DHT.h>

// DHT22 setup
#define DHTPIN D8         // Change this to the pin you're using for DHT22
#define DHTTYPE DHT22    // DHT22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// Use your own UUIDs here
#define SERVICE_UUID        "53fac100-ad46-4ca6-b8bb-26cb1ebe3af6"
#define CHARACTERISTIC_UUID "6325e571-ad46-4c13-9088-6762baf82ff8"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
    }
};

void setup() {
    Serial.begin(115200);
    dht.begin();

    // Initialize BLE
    BLEDevice::init("ClimaSphere_Server_Device");
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    BLEService *pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_WRITE |
        BLECharacteristic::PROPERTY_NOTIFY
    );
    pCharacteristic->addDescriptor(new BLE2902());

    pService->start();
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
    Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
    if (!deviceConnected) {
        delay(500); // Small delay to not flood the output
        return;
    }

    // Read humidity from DHT22
    float humidity = dht.readHumidity();
    // Check if read failed and exit early (to try again).
    if (isnan(humidity)) {
        Serial.println("Failed to read from DHT sensor!");
    } else {
        // Print the humidity value to the Serial Monitor
        Serial.print("Hello");
        Serial.print("Humidity: ");
        Serial.print(humidity);
        Serial.println("%");

        // Convert humidity value to a string to send over BLE
        char buf[16];
        snprintf(buf, sizeof(buf), "%f%%", humidity);
        pCharacteristic->setValue(buf);
        pCharacteristic->notify();
        Serial.print("Notify value: ");
        Serial.println(buf);
    }

    // Handling the BLE connection status
    if (!deviceConnected && oldDeviceConnected) {
        pServer->startAdvertising(); // Restart advertising
        Serial.println("Start advertising");
        oldDeviceConnected = deviceConnected;
    }
    if (deviceConnected && !oldDeviceConnected) {
        oldDeviceConnected = deviceConnected;
    }

    delay(1000); // Delay between reads for stability
}
