#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char *ssid     = "Internet Puhalo";
const char *password = "V8UpvvZoic";

// Get timezone from https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
#define NTP_SERVER "pool.ntp.org"
#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"

// #define UTC_OFFSET_IN_SECONDS 7200
// #define UPDATE_TIME_EVERY_MILLIS 1 * 60 * 1000

#define LED_PIN 2

unsigned long _lastUpdatedTime = 0;
unsigned long _wifiConnectionTimer = 0; 

// WiFiUDP ntpUDP;
// NTPClient timeClient(ntpUDP, "pool.ntp.org", UTC_OFFSET_IN_SECONDS);

void setup(){
  Serial.begin(115200);
  delay(2000);

  Serial.println("<dbg>Connecting to WiFi");

  pinMode(LED_PIN, OUTPUT);

  configTime(TIMEZONE, NTP_SERVER);

  connectToWifi();

  Serial.println("</dbg>}");

  // timeClient.begin();
}

void loop() {

  // if (timePeriodIsOver(_lastUpdatedTime, UPDATE_TIME_EVERY_MILLIS)) {
  //   timeClient.update();    
  // }

  // String formattedTime = timeClient.getFormattedTime();
  // Serial.print("Formatted Time: ");
  // Serial.println(formattedTime);  

  printTime();
  delay(1000);
}

void printTime() {
  char printValue[8];
  struct tm timeinfo;

  if (!getLocalTime(&timeinfo)){
    Serial.println("<dbg>Failed to obtain time.</dbg>}");
    blinkLed(50);
    blinkLed(50);
    return;
  }

  sprintf(printValue, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  Serial.println("{" + String(printValue) + "};");

  blinkLed(50);
}

void blinkLed(int delayTime) {
  digitalWrite(LED_PIN, LOW);
  delay(delayTime);
  digitalWrite(LED_PIN, HIGH);
  delay(delayTime);
}

void indicateConnectingToWiFi() {
  digitalWrite(LED_PIN, LOW);
  delay(1000);

  blinkLed(100);
  blinkLed(100);
  blinkLed(100);
  blinkLed(100);
}

void connectToWifi() {
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    yield();

    if (timePeriodIsOver(_wifiConnectionTimer, 500)) {
      Serial.print(".");
      indicateConnectingToWiFi();
    }
  }

  Serial.println(".");

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected to network: ");
    Serial.println(WiFi.localIP());
  }
}

boolean timePeriodIsOver(unsigned long & periodStartTime, unsigned long timePeriod) {
  unsigned long currentMillis  = millis();
  if ( currentMillis - periodStartTime >= timePeriod )
  {
    periodStartTime = currentMillis; // set new expireTime
    return true;                // more time than TimePeriod) has elapsed since last time if-condition was true
  }
  else return false;            // not expired
}
