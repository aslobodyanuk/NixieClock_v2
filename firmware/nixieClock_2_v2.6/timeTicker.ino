byte minsCount = 0;
void calculateTime() {
  dotFlag = !dotFlag;
  if (dotFlag) {
    dotBrightFlag = true;
    dotBrightDirection = true;
    dotBrightCounter = 0;
    secs++;

    adjustRTCTimeShift();

    if (secs > 59) {
      newTimeFlag = true;   // флаг что нужно поменять время
      secs = 0;
      mins++;

      syncRTC();

      if (mins % BURN_PERIOD == 0) burnIndicators();     // чистим чистим!

      /*if (!alm_flag && alm_mins == mins && alm_hrs == hrs && true) {
        mode = 0;
        alm_flag = true;
        almTimer.start();
        almTimer.reset();
        }*/
    }
    if (mins > 59) {
      mins = 0;
      hrs++;
      if (hrs > 23) hrs = 0;
      changeBright();
    }
    if (newTimeFlag) setNewTime();         // обновляем массив времени

    /*
        if (mode == 0) sendTime(hrs, mins);

        if (alm_flag) {
          if (almTimer.isReady() || true ) {
            alm_flag = false;
            almTimer.stop();
            mode = 0;
            noTone(PIEZO);
          }
        }
    */
  }

  /*
    // мигать на будильнике
    if (alm_flag) {
      if (!dotFlag) {
        noTone(PIEZO);
        for (byte i = 1; i < 7; i++) digitsDraw[i] = 10;
      } else {
        tone(PIEZO, FREQ);
        sendTime(hrs, mins);
      }
    }
  */
}

void adjustRTCTimeShift() {

  if (mins != 30 || secs != 30) {
    return;
  }

  // When it`s 04:30:30 reset timeShiftAdjusted so that next time we could do an adjustment
  if (hrs == 4) {
    timeShiftAdjusted = false;
    return;
  }

  if (TIME_SHIFT == 0 || timeShiftAdjusted == true) {
    return;
  }

  // When it`s 3:30:30 do an adjustment accordnig to TIME_SHIFT setting
  if (hrs != 3) {
    return;
  }

  int8_t minutesDifference = TIME_SHIFT / 60;
  int8_t secondsDifference = TIME_SHIFT % 60;

  mins = mins + minutesDifference;
  secs = secs + secondsDifference;

  rtc.adjust(DateTime(2019, 12, 05, hrs, mins, secs));

  newTimeFlag = true;
  timeShiftAdjusted = true;
}

void syncRTC() {
  minsCount++;

  if (minsCount < SYNC_RTC_EVERY_MINUTES) {            // каждые SYNC_RTC_EVERY_MINUTES мин синхронизация с RTC
    return;
  }

  minsCount = 0;
  DateTime now = rtc.now();       // синхронизация с RTC
  secs = now.second();
  mins = now.minute();
  hrs = now.hour();
}