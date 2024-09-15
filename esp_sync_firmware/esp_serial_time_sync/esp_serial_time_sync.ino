#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <SimplePortal.h>
#include <FileData.h>
#include <LittleFS.h>

// Get timezone from https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
#define NTP_SERVER "pool.ntp.org"
#define TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"

#define LED_PIN 2

#define CONFIG_PORTAL_TIMEOUT_MS 60000

#define STATUS_CONFIG_PORTAL 0
#define STATUS_WIFI_CONNECTING 1
#define STATUS_WIFI_CONNECT 2

unsigned long _lastUpdatedTime = 0;
unsigned long _wifiConnectionTimer = 0;
unsigned long _configPortalTimer = 0;
unsigned long _configPortalTimoutTimer = 0;
byte _status = STATUS_CONFIG_PORTAL;

struct ConnectionData {
  char ssid[32];
  char pswd[32];
};
ConnectionData _connectionData;

FileData _connectionFileData(&LittleFS, "/conn_data.dat", 'B', &_connectionData, sizeof(_connectionData));

void setup(){
  Serial.begin(115200);
  delay(3000);
  Serial.println("Initializing ESP.");

  pinMode(LED_PIN, OUTPUT);
  initializeConnectionData();
  configTime(TIMEZONE, NTP_SERVER);
  portalStart();
}

void loop() {
  _connectionFileData.tick();

  if (_status == STATUS_CONFIG_PORTAL && timePeriodIsOver(_configPortalTimer, 500)) {
    indicatePortal();
  }

  if (_status == STATUS_CONFIG_PORTAL && timePeriodIsOver(_configPortalTimoutTimer, CONFIG_PORTAL_TIMEOUT_MS)) {
    Serial.println("Configuration portal timeout.");
    connectToWifi();
  }

  if (_status == STATUS_CONFIG_PORTAL && portalTick()) {
    connectToWifi();
  }

  if (_status == STATUS_WIFI_CONNECT) {
    printTime();
    delay(1000);
  }
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

void indicatePortal() {
  blinkLed(500);
}

void initializeConnectionData() {
  LittleFS.begin();
  _connectionFileData.read();
  printConnectionData();
}

void connectToWifi() {
  _status = STATUS_WIFI_CONNECTING;

  Serial.print("Portal status: ");
  Serial.println(portalStatus());
  // status: 0 error, 1 connect, 2 ap, 3 local, 4 exit, 5 timeout
  portalStop();
  
  if (portalStatus() == SP_SUBMIT) {
    Serial.println("Got new WiFi config from portal.");
    Serial.println(portalCfg.SSID);
    Serial.println(portalCfg.pass);

    strcpy(_connectionData.ssid, portalCfg.SSID);
    strcpy(_connectionData.pswd, portalCfg.pass);
    _connectionFileData.updateNow();

    WiFi.mode(WIFI_STA);
    WiFi.begin(portalCfg.SSID, portalCfg.pass);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
  }

  if (portalStatus() != SP_SUBMIT) {
    Serial.println("Connecting to previous WiFi.");
    printConnectionData();

    WiFi.mode(WIFI_STA);
    WiFi.begin(_connectionData.ssid, _connectionData.pswd);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
  }

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

  _status = STATUS_WIFI_CONNECT;
}

void printConnectionData() {
  Serial.print("Saved ssid: ");
  Serial.println(_connectionData.ssid);

  Serial.print("Saved pswd: ");
  Serial.println(_connectionData.pswd);
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
