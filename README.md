# Plant-Humidity-Sensors
Reads multiple plant humidity sensors over wifi and graphs the results
This project reads plant humidty sensors attached to a Nano 33 iot, via wifi.
The microprocessor code is in c++ and concts to the server over UDP.
The server is in python and manages the conected Nano 33 using json config messages. The config is loaded from file and fully configurable.
The project is currently expanding to:
1)  Use multiple Nano 33 iot processors each with sensors. (Now available)
2) Incude a watering system also attached to Nano 33 iot processors to manage soil humidity levels. This will introduce a new action concept : "watering to the server and device code as well as new hardware. Each plant will be watered via micro servo motors attached to the Nano devices via a PCA9685 16 channel PWM I2C motor driver.  This will free up analog pins on the Nano for additional sensors and drive the motors from a separate power source. The watering and sensor reading of each plant will be independant of the underlying Nano 33s used. In otherwords plants sensors and water motors can be ran off different Nano 33s.
3) Create an App to display the graphs and configure the system via a browser or phone.
