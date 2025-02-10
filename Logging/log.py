import csv # for importing and working with .csv files
import serial # for connecting to machine bluetooth port
import time 

#Logging data from arduino

# Change to your Bluetooth COM port (e.g., "COM6" for Windows, "/dev/rfcomm0" for Linux)
bluetooth_port = "COM6"
baud_rate = 9600
file_name = "sensor_bluetooth_data.csv"

try:
    # Connect to Bluetooth serial port
    ser = serial.Serial(bluetooth_port, baud_rate)
    time.sleep(2)  # Wait for connection to establish

    print(f"Connected to {bluetooth_port}. Logging data...")

    # Open CSV file for writing
    with open(file_name, mode="w", newline="") as file:
        writer = csv.writer(file)
        writer.writerow(["Time (s)", "Temperature (°C)", "Humidity (%)"])  # Header

        while True:
            try:
                data = ser.readline().decode().strip()  # Read Bluetooth data
                if data:
                    print("Received:", data)
                    writer.writerow(data.split(","))  # Write to CSV
            except KeyboardInterrupt:
                print("\nLogging stopped.")
                break

    ser.close()  # Close Bluetooth connection

except Exception as e:
    print("Error:", e)
