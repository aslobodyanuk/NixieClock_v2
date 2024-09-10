#include <AsyncStream.h>
#include <GyverOLED.h>

#define UPDATE_SCREEN_INTERVAL 1000
#define LED_PIN 13

#define MAX_SERIAL_COMMAND_SIZE 20
#define MAX_TIME_LENGTH 10

#define END_TERMINATOR_CHAR ';'

AsyncStream<MAX_SERIAL_COMMAND_SIZE> _asyncSerial(&Serial, END_TERMINATOR_CHAR);
GyverOLED<SSD1306_128x64, OLED_BUFFER> _oled;

int8_t _syncHrs, _syncMins, _syncSecs;
bool _isSyncTimeEmpty = true;

unsigned long _lastUpdatedScreen = 0;

void initializeSerialTimeSync() {
  _oled.init();
  
  Serial.println(F("Initializing serial time sync..."));

  pinMode(LED_PIN, OUTPUT);
}

void serialTimeSyncTick() {
  readTimeFromSerial();

  if (millis() > _lastUpdatedScreen + UPDATE_SCREEN_INTERVAL) {
    updateScreen();
    _lastUpdatedScreen = millis();
  }
}

bool readTimeFromSerial() {
  if (_asyncSerial.available()) {
    
    char rawTime[MAX_TIME_LENGTH];

    if (strlen(_asyncSerial.buf) < MAX_TIME_LENGTH) {
      Serial.print(F("Skipping message, because it`s too short: "));
      Serial.println(String(_asyncSerial.buf));
      return false;
    }

    char* lastOccurence = strrchr(_asyncSerial.buf, '{');
    Serial.print(F("Last Occurence: "));
    Serial.println(String(lastOccurence));

    if (lastOccurence == NULL) {
      Serial.print(F("Could not find starting character in input: "));
      Serial.println(String(_asyncSerial.buf));
      _isSyncTimeEmpty = true;
      return false;
    }

    strncpy(rawTime, lastOccurence, MAX_TIME_LENGTH);
    Serial.print(F("RawTime: "));
    Serial.println(String(rawTime));

    if (!(rawTime[0] == '{' && rawTime[MAX_TIME_LENGTH - 1] == '}')) {
      Serial.print(F("Could not parse time string in input: "));
      Serial.println(String(_asyncSerial.buf));
      _isSyncTimeEmpty = true;
      return false;
    }

    parseTimeString(rawTime);
    Serial.print(F("Current time: "));
    Serial.println(getPrintableTime());
    return true;
  }
}

void updateScreen() {
  _oled.clear();
  _oled.home();
  _oled.setScale(2);

  String printValue = getPrintableTime();
  _oled.print(printValue);

  _oled.setCursorXY(0, 20);
  _oled.print(F("IsE: "));
  _oled.print(String(_isSyncTimeEmpty));

  _oled.setCursorXY(0, 40);
  _oled.print(String(_syncHrs) + ":" + String(_syncMins) + ":"  + String(_syncSecs));
  
  _oled.update();
}

String getPrintableTime() {
  return String(hrs) + ":" + String(mins) + ":"  + String(secs);
}

String padTime(byte number) {
  if (number < 10) {
    return String(F("0")) + String(number);
  }

  return String(number);
}

void parseTimeString(char input[]) {
  char hours[] = { input[1], input[2], '\0' };
  char minutes[] = { input[4], input[5], '\0' };
  char seconds[] = { input[7], input[8], '\0' };

  _syncHrs = atoi(hours);
  _syncMins = atoi(minutes);
  _syncSecs = atoi(seconds);
  _isSyncTimeEmpty = false;
}

void blinkLed(int delayTime) {
  digitalWrite(LED_PIN, LOW);
  delay(delayTime);
  digitalWrite(LED_PIN, HIGH);
  delay(delayTime);
}
