import serial
import csv
import re

# Function to parse incoming sensor data
def parse_sensor_data(line):
    try:
        # Extract key-value pairs using regex
        data_dict = {}
        parts = re.findall(r"(\S+):([-+]?[0-9]*\.?[0-9]+)", line)
        
        for key, value in parts:
            data_dict[key] = float(value)
        
        # Ensure all fields are present in the correct order
        return [data_dict.get(field, 0.0) for field in FIELDS]
    except Exception as e:
        print(f"Error parsing line: {line}, Error: {e}")
        return None

# Serial port configuration
SERIAL_PORT = "COM3"  # Adjust to match your device
BAUD_RATE = 9600
TIMEOUT = 1

# CSV file path
# FILENAME = "C:/Users/suhas/Desktop/bikshu_copy_20250209223039/walking.csv"
#FILENAME = "C:/Users/suhas/Desktop/bikshu_copy_20250209223039/sitting.csv"
FILENAME = "C:/Users/suhas/Desktop/bikshu_copy_20250209223039/pendulum.csv"

# Define field names for CSV output
FIELDS = [
    "Seq", "Acc_X_Min", "Acc_X_Max", "Acc_X_Avg", "Acc_X_Variance", "Acc_X_StdDev", "Acc_X_Range", "Acc_X_Skew", "Acc_X_Kurtosis", "Acc_X_Energy",
    "Acc_Y_Min", "Acc_Y_Max", "Acc_Y_Avg", "Acc_Y_Variance", "Acc_Y_StdDev", "Acc_Y_Range", "Acc_Y_Skew", "Acc_Y_Kurtosis", "Acc_Y_Energy",
    "Acc_Z_Min", "Acc_Z_Max", "Acc_Z_Avg", "Acc_Z_Variance", "Acc_Z_StdDev", "Acc_Z_Range", "Acc_Z_Skew", "Acc_Z_Kurtosis", "Acc_Z_Energy",
    "Gyro_X_Min", "Gyro_X_Max", "Gyro_X_Avg", "Gyro_X_Variance", "Gyro_X_StdDev", "Gyro_X_Range", "Gyro_X_Skew", "Gyro_X_Kurtosis", "Gyro_X_Energy",
    "Gyro_Y_Min", "Gyro_Y_Max", "Gyro_Y_Avg", "Gyro_Y_Variance", "Gyro_Y_StdDev", "Gyro_Y_Range", "Gyro_Y_Skew", "Gyro_Y_Kurtosis", "Gyro_Y_Energy",
    "Gyro_Z_Min", "Gyro_Z_Max", "Gyro_Z_Avg", "Gyro_Z_Variance", "Gyro_Z_StdDev", "Gyro_Z_Range", "Gyro_Z_Skew", "Gyro_Z_Kurtosis", "Gyro_Z_Energy",
    "Mag_X_Min", "Mag_X_Max", "Mag_X_Avg", "Mag_X_Variance", "Mag_X_StdDev", "Mag_X_Range", "Mag_X_Skew", "Mag_X_Kurtosis", "Mag_X_Energy",
    "Mag_Y_Min", "Mag_Y_Max", "Mag_Y_Avg", "Mag_Y_Variance", "Mag_Y_StdDev", "Mag_Y_Range", "Mag_Y_Skew", "Mag_Y_Kurtosis", "Mag_Y_Energy",
    "Mag_Z_Min", "Mag_Z_Max", "Mag_Z_Avg", "Mag_Z_Variance", "Mag_Z_StdDev", "Mag_Z_Range", "Mag_Z_Skew", "Mag_Z_Kurtosis", "Mag_Z_Energy"
]

# Initialize serial connection
ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=TIMEOUT)

# Open CSV file and write header
with open(FILENAME, "w", encoding="UTF8", newline="") as f:
    writer = csv.writer(f)
    writer.writerow(FIELDS)

    print(f"Collecting data from {SERIAL_PORT}...")
    count = 0
    while count < 5100:
        try:
            read_ser = ser.readline().decode("utf-8").strip()
            if read_ser.startswith("Seq:"):
                print("Received:", read_ser)
                data = parse_sensor_data(read_ser)
                if data:
                    writer.writerow(data)
                    f.flush()  # Ensure immediate writing to file
        except KeyboardInterrupt:
            print("Data collection stopped.")
            break
        count += 1

print("Data collection complete.")
ser.close()
