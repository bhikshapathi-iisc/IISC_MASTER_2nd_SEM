#include <Arduino_LSM9DS1.h>
#include "exported_model.h"  // Ensure this header has the correct model

const int N = 5; // Buffer size for feature calculation
int sequence = 0;

// Buffers to store sensor data
float Ax[N] = {0}, Ay[N] = {0}, Az[N] = {0};
float Gx[N] = {0}, Gy[N] = {0}, Gz[N] = {0};
float Mx[N] = {0}, My[N] = {0}, Mz[N] = {0};

// Structure for Mean Calculation
struct Stats {
  float mean;
};

// Mean Stats
Stats accX, accY, accZ, gyroX, gyroY, gyroZ, magX, magY, magZ;

// Buffer Count
bool buffersFilled = false;
int bufferCount = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU");
    while (1);
  }

  Serial.println("IMU Initialized. Running real-time activity detection...");
}

// Function to Read IMU Sensor Data
void readIMU(float &ax, float &ay, float &az, float &gx, float &gy, float &gz, float &mx, float &my, float &mz) {
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(ax, ay, az);
  }
  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(gx, gy, gz);
  }
  if (IMU.magneticFieldAvailable()) {
    IMU.readMagneticField(mx, my, mz);
  }
}

// Function to Update Buffer and Compute Mean
void updateBuffer(float buffer[], Stats &stats, float newVal) {
  float sum = 0;

  // Shift buffer values
  for (int i = 0; i < N - 1; i++) {
    buffer[i] = buffer[i + 1];
    sum += buffer[i];
  }
  
  buffer[N - 1] = newVal;
  sum += newVal;

  // Compute Mean
  stats.mean = sum / N;
}

// Function to Convert Features into Model Input
void getFeatureVector(float featureVector[]) {
  featureVector[0] = accX.mean;
  featureVector[1] = accY.mean;
  featureVector[2] = accZ.mean;
  
  featureVector[3] = gyroX.mean;
  featureVector[4] = gyroY.mean;
  featureVector[5] = gyroZ.mean;
  
  featureVector[6] = magX.mean;
  featureVector[7] = magY.mean;
  featureVector[8] = magZ.mean;
}

// Function to find the index of the maximum value in an array
int argmax(float arr[], int size) {
  int maxIndex = 0;
  for (int i = 1; i < size; i++) {
    if (arr[i] > arr[maxIndex]) {
      maxIndex = i;
    }
  }
  return maxIndex;
}

void loop() {
  float ax, ay, az, gx, gy, gz, mx, my, mz;
  readIMU(ax, ay, az, gx, gy, gz, mx, my, mz);

  updateBuffer(Ax, accX, ax);
  updateBuffer(Ay, accY, ay);
  updateBuffer(Az, accZ, az);
  updateBuffer(Gx, gyroX, gx);
  updateBuffer(Gy, gyroY, gy);
  updateBuffer(Gz, gyroZ, gz);
  updateBuffer(Mx, magX, mx);
  updateBuffer(My, magY, my);
  updateBuffer(Mz, magZ, mz);

  if (bufferCount < N) {
    bufferCount++;
  } else {
    buffersFilled = true;
  }

  if (buffersFilled) {
    float featureVector[9];  // Mean values
    getFeatureVector(featureVector);

    // Run the model prediction
    float prediction[5];
    score(featureVector, prediction); // Call updated score function

    // Determine the predicted class based on the highest probability
    int predictedClass = argmax(prediction, 5);

    // Map class to activity labels
    String activityLabels[5] = {"sitting", "swinging" ,"typing", "walking", "writing"};
    String activity = activityLabels[predictedClass];

    Serial.print("Detected Activity: ");
    Serial.println(activity);

    sequence++;
  }

  delay(1000);  // Adjust delay based on real-time performance needs
}

