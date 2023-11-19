#define BLYNK_TEMPLATE_ID "TMPL3Gg35zR1R"
#define BLYNK_TEMPLATE_NAME "dustbin"
#define BLYNK_AUTH_TOKEN "Z1tvydtoM1ayvGhturYZcB5cL85Gll73"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>


char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "realme";
char pass[] = "e27cdziq";
const char* webhookURL = "https://maker.ifttt.com/trigger/emailalert/with/key/d3IJRGQjQrRYC7WVevfdqI";

BlynkTimer timer;

#define echoPin D5
#define trigPin D7
#define motionPin D8

Servo servo;
long duration;
int distance;
int binLevel = 0;


int initialServoPosition = 0;
unsigned long lastMotionTime = 0;
bool isMotorSpinning = false;



void motionSensor() {
  int motionState = digitalRead(motionPin);
  unsigned long currentTime = millis();

  if (motionState == HIGH) {
    // Motion detected
    if (!isMotorSpinning) {
      isMotorSpinning = true;
      initialServoPosition = servo.read();
      servo.write(360); 
      Blynk.virtualWrite(V2, 360); 
      lastMotionTime = currentTime;
    }
  } else {
    // No motion detected
    if (isMotorSpinning && (currentTime - lastMotionTime >= 8000)) {
      // If motor spinning and 8 seconds have passed, return to the initial position
      isMotorSpinning = false;
      servo.write(initialServoPosition);
      Blynk.virtualWrite(V2, initialServoPosition); // Update Blynk widget
    }
  }
}


void ultrasonic()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2); // Added delay here
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; // formula to calculate the distance for the ultrasonic sensor
  binLevel = map(distance, 21, 0, 0, 100);
  Blynk.virtualWrite(V0, distance);
  Blynk.virtualWrite(V1, binLevel);

  if (binLevel >= 80) {
    sendWebhook();
    delay(30000); // Delay to avoid sending multiple requests quickly
  }
 
  // Check if bin level is at or above 90%
 
}


void sendWebhook() {
  WiFiClient client;
  HTTPClient http;
  http.begin(client,webhookURL);
  int httpResponseCode = http.POST("");

  if (httpResponseCode > 0) {
    Serial.println("Webhook request sent successfully");
  } else {
    Serial.print("Webhook request failed with error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void setup()
{
  Serial.begin(9600);
  servo.attach(D2);
  pinMode(D8, INPUT_PULLUP); // Set motionPin as INPUT_PULLUP
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Blynk.begin(auth, ssid, pass);
  delay(2000);
  // timer.setInterval(1000L, motionSensor);
  timer.setInterval(1000L, ultrasonic);
}

void loop()
{
  Blynk.run();
  timer.run();
  motionSensor(); //define MontionSensor to  loop for Montion Detection. 
}