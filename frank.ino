#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// WIFI
const char* ssid = "STARLINK V3";
const char* password = "Kivuhills@2025";

// SERVER URL
const char* serverName = "http://192.168.1.179/ultrasonic_project/save_sensor.php";

// ULTRASONIC PINS
#define TRIG D5
#define ECHO D6

// LED PINS
#define RED_LED     D1
#define YELLOW_LED  D2
#define GREEN_LED   D7

WiFiClient client;

long duration;
float currentDistance = 0;
float previousDistance = 0;

void setup() {

  Serial.begin(115200);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  // LEDs OFF
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);

  // CONNECT WIFI
  WiFi.begin(ssid, password);

  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi Connected");

  Serial.print("ESP IP: ");
  Serial.println(WiFi.localIP());
}

/*
   GET DISTANCE
*/
float getDistance() {

  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH, 30000);

  // NO PULSE
  if (duration == 0) {
    return -1;
  }

  float distance = duration * 0.0343 / 2;

  return distance;
}

/*
   UPDATE LEDs
*/
void updateLEDs(float dist) {

  // TURN OFF ALL LEDs
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);

  // RED
  if (dist > 0 && dist <= 10) {

    digitalWrite(RED_LED, HIGH);

  }

  // YELLOW
  else if (dist > 10 && dist <= 20) {

    digitalWrite(YELLOW_LED, HIGH);

  }

  // GREEN
  else {

    digitalWrite(GREEN_LED, HIGH);

  }
}

/*
   SEND DATA TO SERVER
*/
void sendData(float dist, bool motion) {

  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;

    http.begin(client, serverName);

    http.addHeader("Content-Type", "application/json");

    // JSON DATA
    String jsonData = "{";
    jsonData += "\"distance\":";
    jsonData += String(dist, 2);
    jsonData += ",";
    jsonData += "\"motion\":";
    jsonData += motion ? "true" : "false";
    jsonData += "}";

    Serial.print("Sending: ");
    Serial.println(jsonData);

    int httpResponseCode = http.POST(jsonData);

    Serial.print("HTTP Response: ");
    Serial.println(httpResponseCode);

    if (httpResponseCode > 0) {

      String response = http.getString();

      Serial.print("Server Response: ");
      Serial.println(response);

    } else {

      Serial.print("Error: ");
      Serial.println(http.errorToString(httpResponseCode));

    }

    http.end();

  } else {

    Serial.println("WiFi Disconnected");

  }
}

void loop() {

  currentDistance = getDistance();

  // SENSOR ERROR
  if (currentDistance < 0) {

    Serial.println("No object detected");

    delay(2000);

    return;
  }

  /*
     MOTION DETECTION
  */
  bool motionDetected = false;

  float difference = abs(currentDistance - previousDistance);

  if (difference > 5) {

    motionDetected = true;

  }

  // UPDATE LEDs
  updateLEDs(currentDistance);

  // FORCE NO MOTION IF FAR
  if (currentDistance > 30) {

    motionDetected = false;

  }

  // SERIAL OUTPUT
  Serial.print("Distance: ");
  Serial.print(currentDistance);
  Serial.println(" cm");

  Serial.print("Motion: ");

  if (motionDetected) {
    Serial.println("YES");
  } else {
    Serial.println("NO");
  }

  Serial.println("----------------------");

  // SEND TO SERVER
  sendData(currentDistance, motionDetected);

  // SAVE PREVIOUS VALUE
  previousDistance = currentDistance;

  delay(3000);
}
