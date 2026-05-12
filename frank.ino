#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "STARLINK V3";
const char* password = "Kivuhills@2025";

/*
   YOUR COMPUTER IP ADDRESS
*/
String serverName =
"http://192.168.1.179/ultrasonic_project/save_sensor.php";

#define TRIG D5
#define ECHO D6

WiFiClient client;

long duration;
float distance = 0;

float previousDistance = 0;

void setup() {

  Serial.begin(115200);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  WiFi.begin(ssid, password);

  Serial.println("");
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Connected");

  Serial.print("ESP IP Address: ");
  Serial.println(WiFi.localIP());
}

/*
    READ DISTANCE
*/
float getDistance() {

  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH, 30000);

  /*
      HANDLE NO ECHO
  */
  if (duration == 0) {
    return -1;
  }

  distance = duration * 0.034 / 2;

  return distance;
}

/*
    SEND DATA TO PHP SERVER
*/
void sendData(float dist, bool motion) {

  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;

    Serial.println("Sending data to server...");

    http.begin(client, serverName);

    http.addHeader("Content-Type", "application/json");

    String jsonData = "{";
    jsonData += "\"distance\":" + String(dist, 2) + ",";
    jsonData += "\"motion\":" + String(motion ? "true" : "false");
    jsonData += "}";

    Serial.println("JSON:");
    Serial.println(jsonData);

    int responseCode = http.POST(jsonData);

    Serial.print("HTTP Response Code: ");
    Serial.println(responseCode);

    /*
        PRINT SERVER RESPONSE
    */
    String response = http.getString();

    Serial.println("Server Response:");
    Serial.println(response);

    http.end();

  } else {

    Serial.println("WiFi Disconnected");
  }
}

void loop() {

  float currentDistance = getDistance();

  /*
      INVALID SENSOR READING
  */
  if (currentDistance < 0) {

    Serial.println("No object detected");

    delay(2000);
    return;
  }

  bool motionDetected = false;

  /*
      MOTION DETECTION
  */
  if (abs(currentDistance - previousDistance) > 8) {

    motionDetected = true;
  }

  Serial.print("Distance: ");
  Serial.print(currentDistance);
  Serial.println(" cm");

  Serial.print("Motion: ");

  if (motionDetected) {
    Serial.println("YES");
  } else {
    Serial.println("NO");
  }

  sendData(currentDistance, motionDetected);

  previousDistance = currentDistance;

  Serial.println("------------------");

  delay(3000);
}