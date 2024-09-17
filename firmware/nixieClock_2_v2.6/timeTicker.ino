byte minsCount = 0;
void calculateTime() {
  secs++;

  if (secs > 59) {
    newTimeFlag = true;   // флаг что нужно поменять время
    secs = 0;
    mins++;

    syncRTC();

    if (mins % BURN_PERIOD == 0) burnIndicators();     // чистим чистим!

  }

  if (mins > 59) {
    mins = 0;
    hrs++;
    if (hrs > 23) hrs = 0;
    changeBright();
  }
  
  if (newTimeFlag) setNewTime();         // обновляем массив времени
}

void syncTimeFromSerial() {

  if (_isSyncTimeEmpty) {
    return;
  }

  rtc.adjust(DateTime(2019, 12, 05, _syncHrs, _syncMins, _syncSecs));
}

void syncRTC() {
  minsCount++;

  if (minsCount < SYNC_RTC_EVERY_MINUTES) {            // каждые SYNC_RTC_EVERY_MINUTES мин синхронизация с RTC
    return;
  }

  syncTimeFromSerial();

  minsCount = 0;
  DateTime now = rtc.now();       // синхронизация с RTC
  secs = now.second();
  mins = now.minute();
  hrs = now.hour();
}