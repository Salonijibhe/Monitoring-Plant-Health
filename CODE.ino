#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Pin Definitions
#define DHTPIN 2           // DHT11 connected to GPIO 2/D4
#define MOISTUREPIN 13     // Soil Moisture sensor connected to D7
#define MQ135_PIN A0       // MQ135 sensor connected to A0
#define MOTOR_PIN1 16       // Water pump control pin on GPIO 16/D0
#define MOTOR_PIN2 5       // Water pump control pin on GPIO 5/D1

// WiFi Credentials
const char* ssid      = "********";         
const char* password  = "********";            

// Web server on port 80
ESP8266WebServer server(80);

// Array to store DHT11 data
uint8_t dht_data[5];

// Variables to store soil and motor status
String soilStatus   = "Unknown";
String motorStatus  = "OFF";

// Timing for motor control
unsigned long motorStartTime = 0;
bool motorActive = false;

// MQ135 constants for calculating PPM
#define RLOAD 10.0      // Load resistance in kilo ohms
#define RZERO 76.63     // Calibration resistance at known CO2 PPM level (for CO2 at 400 ppm)
#define PPMCO2 1000     // Reference CO2 concentration in ppm

// Wi-Fi connection timeout (in ms)
const unsigned long wifiTimeout = 10000;  // 10 seconds

// Function to read DHT11 data
bool readDHT11() {
    
    uint8_t lastState = HIGH;
    uint8_t counter = 0;
    uint8_t j = 0, i;

    dht_data[0] = dht_data[1] = dht_data[2] = dht_data[3] = dht_data[4] = 0;

    pinMode(DHTPIN, OUTPUT);
    digitalWrite(DHTPIN, LOW);
    delay(18);    
    digitalWrite(DHTPIN, HIGH);
    delayMicroseconds(40);  
    pinMode(DHTPIN, INPUT);

    for (i = 0; i < 85; i++) {
      counter = 0;
        while (digitalRead(DHTPIN) == lastState) {
          counter++;
          delayMicroseconds(1);
          if (counter == 255) {
            break;
      }
    }

    lastState = digitalRead(DHTPIN);
    if (counter == 255) 
      break;

    if ((i >= 4) && (i % 2 == 0)) {
      dht_data[j / 8] <<= 1;
      
    if (counter > 16) { 
      dht_data[j / 8] |= 1;
      }
      j++;
    }
  }
  return (j >= 40) && (dht_data[4] == ((dht_data[0] + dht_data[1] + dht_data[2] + dht_data[3]) & 0xFF));
}

float getResistance(int raw_adc) {
    return (1023.0 / raw_adc - 1.0) * RLOAD;
}

float getPPM(float rs) {
    return PPMCO2 * pow((rs / RZERO), -1.5);
}

void setup() {
  Serial.begin(115200);
  pinMode(MOISTUREPIN, INPUT);
  pinMode(MOTOR_PIN1, OUTPUT);
  pinMode(MOTOR_PIN2, OUTPUT);

  //initially keep the pump off

  digitalWrite(MOTOR_PIN1, LOW);  
  digitalWrite(MOTOR_PIN2, LOW);  

  // Attempt Wi-Fi connection with timeout
  
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < wifiTimeout) {
    delay(500);
    Serial.print(".");
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Failed to connect to WiFi, rebooting...");
        ESP.restart();
      } else {
        Serial.println("\nConnected to WiFi!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
      }
    
    server.on("/", handleRoot);
    server.begin();
    Serial.println("HTTP server started");
}

void loop() {
    
    server.handleClient();

    int moistureValue = digitalRead(MOISTUREPIN);

    if (moistureValue == LOW) {     // If soil is Wet
        soilStatus = "Wet";
        if (motorActive) {
            digitalWrite(MOTOR_PIN1, LOW);
            digitalWrite(MOTOR_PIN2, LOW);
            motorActive = false;    // Turn OFF the water pump
            motorStatus = "OFF";
        }
    } else {                        // If soil is Dry
        soilStatus = "Dry";
        if (!motorActive) {
            motorActive = true;     // Turn ON the water pump
            motorStatus = "ON";
            digitalWrite(MOTOR_PIN1, HIGH);
            digitalWrite(MOTOR_PIN2, HIGH);
            motorStartTime = millis();
        }
    }

    // Turn the water pump OFF after 5 seconds if it was turned on
    if (motorActive && millis() - motorStartTime >= 5000) {
        digitalWrite(MOTOR_PIN1, LOW);
        digitalWrite(MOTOR_PIN2, LOW);
        motorActive = false;
        motorStatus = "OFF";
    }
}

void handleRoot() {
    if (readDHT11()) {
        float humidity        = dht_data[0];
        float temperature     = dht_data[2];
        
        int mq135Raw          = analogRead(MQ135_PIN);
        float mq135Resistance = getResistance(mq135Raw);
        float co2PPM          = getPPM(mq135Resistance);

        String html = "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'>";
        html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'><title>Terrace Garden Monitoring System</title>";
        html += "<style>";
        html += "body { font-family: 'Roboto', sans-serif; background: linear-gradient(135deg, #4CAF50, #2E8B57); color: #fff; display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; flex-direction: column; animation: fadeIn 1.5s ease-in-out; }";
        html += "h1 { font-size: 3em; color: #fff; margin-bottom: 20px; text-shadow: 3px 3px 8px rgba(0, 0, 0, 0.4); font-weight: bold; letter-spacing: 2px; }";
        html += ".container { max-width: 600px; padding: 30px; background: rgba(0, 0, 0, 0.25); border-radius: 20px; text-align: center; box-shadow: 0 15px 25px rgba(0, 0, 0, 0.4); backdrop-filter: blur(10px); transition: transform 0.5s ease-in-out; }";
        html += ".container:hover { transform: scale(1.05); }";
        html += ".data-box { padding: 20px; border-radius: 12px; margin: 15px 0; background: rgba(255, 255, 255, 0.2); box-shadow: 0 5px 10px rgba(0, 0, 0, 0.3); transition: transform 0.3s, background 0.3s; }";
        html += ".data-box:hover { transform: translateY(-8px); background: rgba(255, 255, 255, 0.3); }";
        html += ".data-label { font-size: 1.4em; color: #ddd; text-transform: uppercase; font-weight: 500; letter-spacing: 1px; }";
        html += ".value { font-weight: bold; color: #FFEB3B; font-size: 1.8em; text-shadow: 1px 1px 3px rgba(0, 0, 0, 0.3); }";
        html += "img { display: block; margin: 20px auto; width: 100%; max-width: 350px; border-radius: 15px; box-shadow: 0 5px 10px rgba(0, 0, 0, 0.2); animation: bounceIn 1s ease-in-out; }";
        html += "@keyframes fadeIn { 0% { opacity: 0; } 100% { opacity: 1; } }";
        html += "@keyframes bounceIn { 0% { transform: translateY(-200px); opacity: 0; } 60% { transform: translateY(30px); opacity: 1; } 100% { transform: translateY(0); } }";
        html += "@media (max-width: 768px) { .container { padding: 20px; } h1 { font-size: 2.2em; } .data-box { padding: 18px; } .data-label { font-size: 1.1em; } .value { font-size: 1.6em; } }";
        html += "</style></head><body>";
        html += "<img src='https://pic.surf/g7' alt='Plants'>";
        html += "<div class='container'>";
        html += "<h1>Terrace Garden Monitoring</h1>";
        html += "<div class='data-box'><span class='data-label'>Temperature: </span><span class='value'>" + String(temperature) + " °C</span></div>";
        html += "<div class='data-box'><span class='data-label'>Humidity: </span><span class='value'>" + String(humidity) + " %</span></div>";
        html += "<div class='data-box'><span class='data-label'>Soil Condition: </span><span class='value'>" + soilStatus + "</span></div>";
        html += "<div class='data-box'><span class='data-label'>Motor Status: </span><span class='value'>" + motorStatus + "</span></div>";
        html += "<div class='data-box'><span class='data-label'>Air Quality (CO2): </span><span class='value'>" + String(co2PPM) + " PPM</span></div>";
        html += "</div></body></html>";


          server.send(200, "text/html", html);
        } else {
          server.send(200, "text/html", "<h1>Failed to read from DHT11 sensor</h1>");
    }
}