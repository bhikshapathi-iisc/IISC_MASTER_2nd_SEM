#include <Arduino_LSM9DS1.h>
#include <math.h>

const int BUFFER_SIZE = 100;
int sequenceNumber = 0;

// Sensor data buffers
float accX[BUFFER_SIZE] = {0}, accY[BUFFER_SIZE] = {0}, accZ[BUFFER_SIZE] = {0};
float gyroX[BUFFER_SIZE] = {0}, gyroY[BUFFER_SIZE] = {0}, gyroZ[BUFFER_SIZE] = {0};
float magX[BUFFER_SIZE] = {0}, magY[BUFFER_SIZE] = {0}, magZ[BUFFER_SIZE] = {0};

struct SensorStats {
    float total, avg, minValue, maxValue, variance, stdDeviation, range, skew, kurtosis, energy;
};

SensorStats accStatsX, accStatsY, accStatsZ;
SensorStats gyroStatsX, gyroStatsY, gyroStatsZ;
SensorStats magStatsX, magStatsY, magStatsZ;

bool dataReady = false;
int currentBufferCount = 0;

void setup() {
    Serial.begin(9600);
    while (!Serial);

    if (!IMU.begin()) {
        Serial.println("Error: IMU sensor not detected");
        while (1);
    }
    Serial.println("IMU Sensor Initialized. Collecting Data...");
}

void fetchIMUData(float &ax, float &ay, float &az, float &gx, float &gy, float &gz, float &mx, float &my, float &mz) {
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

void processSensorData(float dataBuffer[], SensorStats &stats, float newValue) {
    float oldValue = dataBuffer[0];
    stats.total += newValue - oldValue;
    stats.avg = stats.total / BUFFER_SIZE;
    
    for (int i = 0; i < BUFFER_SIZE - 1; i++) {
        dataBuffer[i] = dataBuffer[i + 1];
    }
    dataBuffer[BUFFER_SIZE - 1] = newValue;
    
    stats.minValue = fmin(stats.minValue, newValue);
    stats.maxValue = fmax(stats.maxValue, newValue);
    stats.range = stats.maxValue - stats.minValue;
    
    stats.variance = 0;
    stats.skew = 0;
    stats.kurtosis = 0;
    stats.energy = 0;
    
    for (int i = 0; i < BUFFER_SIZE; i++) {
        float difference = dataBuffer[i] - stats.avg;
        stats.variance += difference * difference;
        stats.skew += pow(difference, 3);
        stats.kurtosis += pow(difference, 4);
        stats.energy += dataBuffer[i] * dataBuffer[i];
    }
    stats.variance /= BUFFER_SIZE;
    stats.stdDeviation = sqrt(stats.variance);
    stats.skew /= BUFFER_SIZE;
    stats.kurtosis = (stats.kurtosis / BUFFER_SIZE) / (stats.variance * stats.variance);
    stats.energy /= BUFFER_SIZE;
}

void loop() {
    float ax, ay, az, gx, gy, gz, mx, my, mz;
    fetchIMUData(ax, ay, az, gx, gy, gz, mx, my, mz);

    processSensorData(accX, accStatsX, ax);
    processSensorData(accY, accStatsY, ay);
    processSensorData(accZ, accStatsZ, az);
    processSensorData(gyroX, gyroStatsX, gx);
    processSensorData(gyroY, gyroStatsY, gy);
    processSensorData(gyroZ, gyroStatsZ, gz);
    processSensorData(magX, magStatsX, mx);
    processSensorData(magY, magStatsY, my);
    processSensorData(magZ, magStatsZ, mz);

    if (currentBufferCount < BUFFER_SIZE) {
        currentBufferCount++;
    } else {
        dataReady = true;
    }

    if (dataReady) {
        Serial.print("Seq:"); Serial.print(sequenceNumber);
        SensorStats statsArray[] = {accStatsX, accStatsY, accStatsZ, gyroStatsX, gyroStatsY, gyroStatsZ, magStatsX, magStatsY, magStatsZ};
        const char* labels[] = {"Acc_X", "Acc_Y", "Acc_Z", "Gyro_X", "Gyro_Y", "Gyro_Z", "Mag_X", "Mag_Y", "Mag_Z"};
        
        for (int i = 0; i < 9; i++) {
            Serial.print(" "); Serial.print(labels[i]); Serial.print("_Min:"); Serial.print(statsArray[i].minValue, 5);
            Serial.print(" "); Serial.print(labels[i]); Serial.print("_Max:"); Serial.print(statsArray[i].maxValue, 5);
            Serial.print(" "); Serial.print(labels[i]); Serial.print("_Avg:"); Serial.print(statsArray[i].avg, 5);
            Serial.print(" "); Serial.print(labels[i]); Serial.print("_Variance:"); Serial.print(statsArray[i].variance, 5);
            Serial.print(" "); Serial.print(labels[i]); Serial.print("_StdDev:"); Serial.print(statsArray[i].stdDeviation, 5);
            Serial.print(" "); Serial.print(labels[i]); Serial.print("_Range:"); Serial.print(statsArray[i].range, 5);
            Serial.print(" "); Serial.print(labels[i]); Serial.print("_Skew:"); Serial.print(statsArray[i].skew, 5);
            Serial.print(" "); Serial.print(labels[i]); Serial.print("_Kurtosis:"); Serial.print(statsArray[i].kurtosis, 5);
            Serial.print(" "); Serial.print(labels[i]); Serial.print("_Energy:"); Serial.print(statsArray[i].energy, 5);
        }
        Serial.println();
        sequenceNumber++;
    }
    delay(10);
}
