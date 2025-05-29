#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>
#include <math.h>
#include <string.h>

// ------------------- Decision Tree Model -------------------
void score(double * input, double * output) {
  double var0[2];
  if (input[2] <= 3.1701650619506836) {
    double tmp[2] = {0.0, 1.0};  // Predict "Closed"
    memcpy(var0, tmp, 2 * sizeof(double));
  } else {
    double tmp[2] = {1.0, 0.0};  // Predict "Open"
    memcpy(var0, tmp, 2 * sizeof(double));
  }
  memcpy(output, var0, 2 * sizeof(double));
}

// ------------------- BLE Setup -------------------
BLEService doorService("180C");  // Custom BLE Service UUID
BLECharacteristic notifyChar("2A56", BLERead | BLENotify, 20);  // Notifiable char

// ------------------- State Variables -------------------
bool wasOpen = false;
int openCloseCount = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // IMU Init
  if (!IMU.begin()) {
    Serial.println("IMU init failed!");
    while (1);
  }

  // BLE Init
  if (!BLE.begin()) {
    Serial.println("BLE failed to start!");
    while (1);
  }

  BLE.setLocalName("DoorNotifier");
  BLE.setAdvertisedService(doorService);
  doorService.addCharacteristic(notifyChar);
  BLE.addService(doorService);
  BLE.advertise();

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("BLE Door Notifier + Counter ready.");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to: ");
    Serial.println(central.address());

    while (central.connected()) {
      float mx, my, mz;

      if (!IMU.readMagneticField(mx, my, mz)) {
        delay(100);
        continue;
      }

      float m_mag = sqrt(mx * mx + my * my + mz * mz);
      double input[4] = { mx, my, mz, m_mag };
      double output[2];
      score(input, output);

      bool isOpen = (output[0] > output[1]);  // OPEN if output[0] > output[1]

      // LED + Serial State
      if (!isOpen) {
        Serial.println("OPEN");
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);
      } else {
        Serial.println("CLOSED");
        digitalWrite(LED_BUILTIN, LOW);
      }

      // Cycle detection
      static bool inCycle = false;
      if (!wasOpen && isOpen) {
        inCycle = true;
      } else if (wasOpen && !isOpen && inCycle) {
        openCloseCount++;
        inCycle = false;

        // Notify over BLE
          char msg[30];
          snprintf(msg, sizeof(msg), "Opened | Count: %d", openCloseCount);
          notifyChar.writeValue((const uint8_t*)msg, strlen(msg));
          Serial.print("Sent via BLE → ");
          Serial.println(msg);

      }

      wasOpen = isOpen;

      // Print total cycles
      Serial.print("Total cycles: ");
      Serial.println(openCloseCount);

      delay(300);
    }

    Serial.println("BLE disconnected.");
  }
}