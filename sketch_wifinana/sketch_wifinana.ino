/*
  WiFi Test for Nano 33 IoT

  Connects to Wifi network and prints IP address on serial monitor

*/

#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <ArduinoJson.h>


StaticJsonBuffer<2056> jsonBuffer;

const int SENSOR_DELAY = 10; // in milliseconds
const char MAX_SENSORS = 10;
const int MESSAGE_LEN = 379;
char ssid[] = "BTWholeHome-6JC7";      // Wifi SSID
char pass[] = "6uNh7exNwnpg";       // Wifi password
char *str;     //str is a pointer variable; it will hold numerical value returned by function
char myData[20]; 
//char packetBuffer[255];
char packetBuffer[379]; 
int status = WL_IDLE_STATUS;
// Define timer
const int TIMER = 60; // in seconds
const int RESET = TIMER - 1;

int counter;
int nbrOfItems = 0;

char configRequest[] = "{\"message\":\"request\", \"type\":\"config\"}";
JsonObject& confReq = jsonBuffer.parseObject(configRequest);


unsigned int localPort = 2390;  // local port to listen on

WiFiUDP Udp;

unsigned long timestamp;
IPAddress remoteIP = IPAddress(192, 168, 0, 19);
unsigned int remotePort = 2390;


// Initialize the Wifi client
WiFiSSLClient client;

// Default sensor values
const char* itemType = "sensor";
int sensor_pin = A0;
const int data_pin = A0;
int power = 12;
const char* sensor = "plant2";
const char* subType = "soilHumidity";
int output_value;
double reading;
double humidity = 1.0;
int i; 

// struct of sensor data
typedef struct {
   const char* itemType;
   const char* subType;
   const char* sname;
   int powerPin;
   int sensor_pin;
   double reading1;
   double reading2; 
} Sensor;
Sensor sensors[MAX_SENSORS];

class SerialComm {
  public:
  int rate;
  bool on = false;

  SerialComm(int r){
    rate = r;
    Serial.begin(r);
    int pass = 0;
    while (!Serial and pass < 10) {
      ; // wait for serial port to connect. Needed for native USB port only
      pass++;
      delay(2000);
    }
    if (Serial){
      on = true;
    }
  }
};

class Message {
  public:
  int nbrOfItems = 0;
  Sensor sensors[MAX_SENSORS];
  char dataMessage[MESSAGE_LEN];
  JsonObject& dataM = jsonBuffer.createObject();

  int get_pin(String gpio, String mode) {
    int pin = 3;
    if (mode == "A") {
      if (gpio == "A0") {
        pin = A0;
      } else if (gpio == "A1" or gpio == "1") {
        pin = A1;
      } else if (gpio == "A2" or gpio == "2") {
        pin = A2;
      } else if (gpio == "A3" or gpio == "3") {
        pin = A3;
      } else if (gpio == "A4" or gpio == "4") {
        pin = A4;
      } else if (gpio == "A5" or gpio == "5") {
        pin = A5;
      } else if (gpio == "A6" or gpio == "6") {
        pin = A6;
      } else if (gpio == "A7" or gpio == "7") {
        pin = A7;
      } else {
        pin = A0;
      }  
    } else if (mode == "D") {
        if (gpio == "D0" or gpio == "0") {
          pin = 0;
      } else if (gpio == "D1" or gpio == "1") {
          pin = 1;
      } else if (gpio == "D2" or gpio == "2") {
          pin = 2;
      } else if (gpio == "D4" or gpio == "4") {
          pin = 4;
      } else if (gpio == "D3" or gpio == "3") {
          pin = 3;
      } else if (gpio == "D4" or gpio == "4") {
          pin = 4;
      } else if (gpio == "D5" or gpio == "5") {
          pin = 5;
      } else if (gpio == "D6" or gpio == "6") {
          pin = 6;
      } else if (gpio == "D7" or gpio == "7") {
          pin = 7;
      } else if (gpio == "D8" or gpio == "8") {
          pin = 8;
      } else if (gpio == "D9" or gpio == "9") {
          pin = 9;
      } else if (gpio == "D10" or gpio == "10") {
          pin = 10;
      } else if (gpio == "D11" or gpio == "11") {
          pin = 11;
      } else if (gpio == "D12" or gpio == "12") {
          pin = 12;
      } else if (gpio == "D13" or gpio == "13") {
          pin = 13;
      } else if (gpio == "D14" or gpio == "14") {
          pin = 14;
      } else if (gpio == "D15" or gpio == "15") {
          pin = 15;
      } else if (gpio == "D16" or gpio == "16") {
          pin = 16;
      } else if (gpio == "D17" or gpio == "17") {
          pin = 17;
      } else if (gpio == "D18" or gpio == "18") {
          pin = 18;
      } else if (gpio == "D19" or gpio == "19") {
          pin = 19;
      } else if (gpio == "D20" or gpio == "20") {
          pin = 20;
      } else if (gpio == "D21" or gpio == "21") {
          pin = 21;
      } else if (gpio == "D15" or gpio == "15") {
          pin = 15;
      } else if (gpio == "D16" or gpio == "16") {
          pin = 16;
      } else {
        pin = 3;
      }
    }
    return pin;
  }

  void set_message_config() {
    dataM["message"] = "transmit";
    dataM["type"] = "data";
    JsonArray& item = dataM.createNestedArray("item");
    if (nbrOfItems > 0) {
      dataM["nbrItems"] = nbrOfItems;
      for (int j=0; j<nbrOfItems; j++) {
        JsonObject&obj1=item.createNestedObject();
        obj1["type"] = sensors[j].itemType;
        obj1["name"] =  sensors[j].sname;
        obj1["dataPin"] = sensors[j].sensor_pin;
        obj1["subType"] = sensors[j].subType;
        obj1["reading1"] = sensors[j].reading1;
        obj1["reading2"] = sensors[j].reading2;
      }
    } else {
      dataM["nbrItems"] = 1;
      JsonObject&obj1=item.createNestedObject();
      dataM["nbrItems"] = 1;
      obj1["type"] = itemType;
      obj1["name"] =  sensor;
      obj1["dataPin"] = sensor_pin;
      obj1["subType"] = subType;
      obj1["reading1"] = 0;
      obj1["reading2"] = 1; 
    }
    dataM.printTo(Serial);
  }
  
  void set_config(JsonObject& confMessage) {
    nbrOfItems = confMessage["nbrItems"];
    for (int j=0; j<nbrOfItems; j++) {
      const char* itemType = confMessage["items"][j]["type"];
      sensors[j].itemType = confMessage["items"][j]["type"];
      sensors[j].subType = confMessage["items"][j]["subType"];
      sensors[j].sname = confMessage["items"][j]["name"];
      sensors[j].powerPin = int(confMessage["items"][j]["power"]);
      sensors[j].reading1 = double(0);
      sensors[j].reading2 = double(1);
      String gpio = confMessage["items"][j]["data"];
      String mode = confMessage["items"][j]["mode"];
      sensors[j].sensor_pin = get_pin(gpio, mode);
    }
    set_message_config();
  }

  void set_message(char buffer[MESSAGE_LEN]) {
    strncpy(dataMessage, buffer, sizeof(dataMessage));
    JsonObject& configIn = jsonBuffer.parseObject(dataMessage); 
    set_config(configIn);
  }

  void take_reading() {
    for (int j=0; j<nbrOfItems; j++) {
      Serial.print("data pin ");
      Serial.println(sensors[j].sensor_pin);
      // Turn sensor ON and wait a moment.
      digitalWrite(sensors[j].powerPin, HIGH);
      delay(SENSOR_DELAY);
      output_value= analogRead(sensors[j].sensor_pin);
      // Turn sensor OFF again and reset.
      digitalWrite(sensors[j].powerPin, LOW);
      Serial.print("Reading ");
      Serial.print(i);
      Serial.print(" : ");
      Serial.println(output_value);
      dataM["item"][j]["reading1"] = output_value;
 
      output_value = map(output_value,1050,0,0,100);
      Serial.print("Mositure : ");
      Serial.print(output_value);
      Serial.println("%");
      dataM["item"][j]["reading2"] = output_value;
      dataM["item"][j]["count"] = i;
    }
    // Send UDP packet
    //Udp.beginPacket(IPAddress(192, 168, 0, 19), remotePort);
    Udp.beginPacket(remoteIP, remotePort);
    dataM.printTo(Udp);
    Udp.println();
    Udp.endPacket();
  }

  bool get_config() {
    bool recive = 0;
    if(!confReq.success()) {
      Serial.println("parseObject() confReq failed");
    }
    Udp.beginPacket(remoteIP, remotePort);
    confReq.printTo(Udp);
    Udp.endPacket();
    // Run search for config message 10 times
    int pass = 0;
    while (!recive and pass < 10) {
      int packetSize = Udp.parsePacket();
      if (packetSize) {
        Serial.print("Received packet of size ");
        Serial.println(packetSize);
        int len = Udp.read(packetBuffer, 379);
        if (len > 0) {
          packetBuffer[len] = '\0';
        }
        JsonObject& confMessage = jsonBuffer.parseObject(packetBuffer);
        if(!confMessage.success()) {
          Serial.println("parseObject() confMessage failed");
          return false;
        }
        set_config(confMessage);
        recive = 1;
      }
      pass = pass+1;
      delay(2000);
    }
    return true;
  }
};

Message messIn;

void setup() {
  //Initialize serial and wait for port to open:
  //set_serial(9600);
  SerialComm serial(9600);
  
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
  connectToAP();    // Connect to Wifi Access Point
  printWifiStatus();
  Udp.begin(localPort);
  Serial.print("remoteIP : ");
  Serial.print(remoteIP);
  Serial.print("  remotePort : ");
  Serial.println(remotePort); 
  
  messIn.get_config();
  pinMode(power, OUTPUT);
  // Start with sensor OFF
  digitalWrite(power, LOW);
  // Setup timer.
  counter = RESET;
  i = 0;
}

void loop() {
  i = i+1;
  Serial.println(counter);
  // If the counter is at the end of a cycle
  // take a new reading, send and reset counter.
  if (counter <= 0) {
    messIn.take_reading();
    counter = RESET;
  } else {
    delay(SENSOR_DELAY);
  }
  counter--;
  delay(1000-SENSOR_DELAY);
}

bool set_serial(int rate){
  Serial.begin(rate);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  return true;
}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP(); // Device IP address
  Serial.print("IP Address: ");
  Serial.println(ip);
}

void connectToAP() {
  // Try to connect to Wifi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
    Serial.println(status);

    // wait 1 second for connection:
    delay(1000);
    Serial.println("Connected...");
  }
}
