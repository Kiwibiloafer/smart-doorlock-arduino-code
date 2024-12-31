Project started on 25 november 2024

# Smart Door Lock IoT Project

This project is an IoT-based smart door lock system developed using the ESP32 microcontroller. The system utilizes RFID, a numpad PIN, and a door sensor for secure access control. Additionally, it connects to a Firebase Realtime Database to monitor the current status of the door.

## Repository Contents

1. **numpaddd**
   - Contains example code demonstrating the usage of a numpad with the ESP32.

2. **rfiddoor**
   - Contains example code demonstrating the usage of an RFID sensor with the ESP32.

3. **offlinecode**
   - Contains the original code for the smart door lock system, designed to work offline without Firebase or internet connectivity.

4. **code**
   - The main code for the smart door lock system, including integration with Firebase.

5. **secrets.h**
   - A header file containing sensitive information such as Wi-Fi credentials, email, API keys, and Firebase configuration details.

## Setup Instructions

1. **Set Up ESP32**
   - Connect the ESP32 microcontroller to your computer using a USB cable.

2. **Set Up Firebase Realtime Database**
   - Configure the database rules and create an authentication account for the project.

3. **Modify `secrets.h`**
   - Update the following variables:
     - Wi-Fi credentials (SSID and password)
     - API keys
     - Firebase database URL
     - Authentication account details

4. **Flash the ESP32**
   - Upload the main code to the ESP32 using your preferred IDE (e.g., Arduino IDE or PlatformIO).

5. **Connect to Firebase**
   - Once flashed, the ESP32 will connect to the Firebase Realtime Database and start reading from and writing data to the database.

## Features

- **Secure Access Control**
  - Access via RFID, PIN input, or door sensor.

- **Real-time Monitoring**
  - The door status is continuously updated in the Firebase Realtime Database.

- **Offline Functionality**
  - Operates without internet connectivity using the `offlinecode`.

- **Customizable Settings**
  - Easily configure sensitive settings in the `secrets.h` file.

## Notes

- Ensure the ESP32 has stable Wi-Fi connectivity to use Firebase.
- Keep `secrets.h` secure to prevent unauthorized access.
- Test individual components (RFID, numpad, door sensor) using the example codes before integrating them into the main system.
