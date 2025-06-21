#  Terrace Garden Monitoring System 

An IoT-based smart system for monitoring and managing your terrace garden using the ESP8266. It provides real-time environmental updates including **temperature**, **humidity**, **soil moisture**, **CO₂ levels**, and **controls a water pump** based on soil conditions — all via a **web dashboard**.

---

##  Features

-  **Temperature & Humidity** monitoring using **DHT11**
-  **Soil Moisture Detection** to determine wet/dry status
-  **Air Quality Monitoring (CO₂ in PPM)** using **MQ135**
-  **Automatic Water Pump Activation** when soil is dry
-  **Live Web Dashboard** accessible over WiFi (hosted by ESP8266)
-  **Responsive and Styled UI** with animations and effects

---

##  Hardware Used

| Component         | Description                                |
|------------------|--------------------------------------------|
| ESP8266 (NodeMCU) | WiFi-enabled microcontroller               |
| DHT11             | Temperature & Humidity Sensor              |
| Soil Moisture     | Analog/digital soil moisture sensor        |
| MQ135             | Air Quality Sensor for CO₂                 |
| Water Pump + Relay| For irrigation control                     |
| Jumper Wires      | Standard female-to-male jumper wires       |
| Breadboard        | For prototyping and connections            |

---

## Circuit Connections

| Component        | ESP8266 Pin        |
|------------------|--------------------|
| DHT11            | D4 / GPIO2          |
| Soil Moisture    | D7 / GPIO13         |
| MQ135            | A0 (Analog Input)   |
| Water Pump       | D0 + D1 / GPIO16 & 5|

---
## IoT Layers
1.	Perception Layer: Sensors collect environmental data on temperature, humidity, soil moisture, and air quality.
2.	Network Layer: NODEMCU facilitates Wi-Fi-based data transfer to the cloud.
3.	Processing Layer: Data is filtered, calibrated, and formatted on the NODEMCU for reliable cloud transmission.
4.	Application Layer: Web-server, stores, and visualizes the data, providing a user-friendly interface for real-time monitoring.


##  Web Interface

Accessible by entering the ESP8266 IP address in a browser after successful Wi-Fi connection.

###  Displayed Info:

-  Temperature (°C)
-  Humidity (%)
-  Soil Condition: `Wet` or `Dry`
-  Water Pump Status: `ON` or `OFF`
-  CO₂ Concentration (PPM)

---

## Wi-Fi Configuration

Update the following lines in your code with your local network credentials:

```cpp

const char* ssid = "Your_SSID";

const char* password = "Your_PASSWORD";

Smart Logic

If Soil is Dry → Motor turns ON for 5 seconds

If Soil is Wet → Motor remains OFF

CO₂ is calculated using MQ135 analog input and resistance conversion

Sensor data is read and updated every HTTP request

Sample Output

Connecting to WiFi...

Connected to WiFi!

IP Address: 192.168.1.10

HTTP server started

Visit http://192.168.1.10 in your browser to view the dashboard.

Getting Started

Flash the code to your NodeMCU using Arduino IDE.

Connect the circuit as per the connections above.

Power the ESP8266 and observe the Serial Monitor.

Once connected to Wi-Fi, open the IP in your browser.

📁 Project Structure

terrace-garden-monitor/

├── terrace_garden.ino       # Main Arduino code

├── README.md                # Project documentation

└── diagrams.png       

Dependencies

Make sure these libraries are installed in your Arduino IDE:

ESP8266WiFi

ESP8266WebServer



