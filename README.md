# Plant-Humidity-Sensors
Reads multiple plant humidity sensors over wifi and graphs the results
This project reads plant humidty sensors attached to a Nano 33 iot, via wifi.
The microprocessor code is in c++ and concts to the server over UDP.
The server is in python and manages the conected Nano 33 using json config messages. The config is loaded from file and fully configurable.
The project is currently expanding to:
1)  Use multiple Nano 33 iot processors each with sensors.
2) Incude a watering system also attached to Nano 33 iot processors to manage soil humidity levels.
