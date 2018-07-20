# Monitor the room and control the servers externally
This part of the project is really important as it is not only will monitor the room by sensors and control the servers externally, but also all the safety measurements will be handled inside the Arduino.

I picked Arduino Mega for the project as it has enough RAM / Flash memory for current and future use.

##### The List of the hardware used:
  - 1x Arduino Mega.
  - 1x Ethernet shield V1 W5100.
  - 1x DHT22 sensor for temperature and humidity.
  - 1x PIR sensor for motion detection.
  - 2x flame sensor for flame detection.
  - 1x QT2 sensor for smoke detection.
  - 1x Hall effect sensor for electricity leaks detection.
  - 1x 4-channel relay.
 
I organized the code into library folder to write each sensor/module into its own class and minimized the code in [main.ino](./main/main.ino) for simplicity.
I used [Arduino Cryptography Library](https://github.com/rweather/arduinolibs) in the implementation of the networking protocol.

##### How to compile the code:
  - Add [Arduino Cryptography Library](https://github.com/rweather/arduinolibs) to the Arduino IDE.
  - Add [library](./library/) to the Arduino IDE.
  - You need to modify [DefinedPins.h](./DefinedPins.h) to your pins.
  - You need to modify some constants and variables in [main.ino](./main/main.ino) to set your network identifier and the private keys to encrypt and decrypt and sign the messages.
  - You also need to modify some other values in [library](./library/) classes to reflect your sensors reading as not all sensors are the same, the code is easy and readable, modifying the code to reflect your own components will be an easy process.
  - Then burn the executable to your Arduino.