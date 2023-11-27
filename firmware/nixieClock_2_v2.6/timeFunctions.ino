// отправить на индикаторы
void sendTime(byte hours, byte minutes) {
  indiDigits[0] = (byte)hours / 10;
  indiDigits[1] = (byte)hours % 10;

  indiDigits[2] = (byte)minutes / 10;
  indiDigits[3] = (byte)minutes % 10;
}

// для эффектов
void setNewTime() {
  newTime[0] = (byte)hrs / 10;
  newTime[1] = (byte)hrs % 10;

  newTime[2] = (byte)mins / 10;
  newTime[3] = (byte)mins % 10;
}

// показать значение типа int на индикаторах (максимальное значение - 9999)
void sendIndicatorValue(int16_t value) {
  int16_t minutes = value % 100;
  int16_t hours = value / 100;

  sendTime(abs(hours), abs(minutes));
}
