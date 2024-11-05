import serial
import time

def check_computation_time(serial_port, baud_rate=9600, start_signal=b'1', stop_signal=b'0'):
    try:
        # Open the serial port
        with serial.Serial(serial_port, baud_rate, timeout=1) as ser:
            print(f"Listening on {serial_port}...")

            # Wait for the start signal
            while True:
                if ser.in_waiting > 0:
                    signal = ser.read().strip()
                    # if signal == start_signal:
                    #     print("start")
                    start_time = time.time()  # Start the timer
                    break

            # Wait for the stop signal
            while True:
                if ser.in_waiting > 0:
                    signal = ser.read().strip()
                    # if signal == stop_signal:
                    #     print("end")
                    end_time = time.time()  # Stop the timer
                    break

            # Calculate and print the elapsed time
            elapsed_time = end_time - start_time
            print(f"Elapsed time: {elapsed_time:.6f} seconds")
            print(f"Elapsed time: {elapsed_time * 1000:.6f} milliseconds")
            
    
    except serial.SerialException as e:
        print(f"Error opening serial port: {e}")

# Usage
serial_port = 'COM8'  # Change to your serial port, e.g., '/dev/ttyUSB0' on Linux

check_computation_time(serial_port)