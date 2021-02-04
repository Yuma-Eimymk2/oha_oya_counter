#include <M5Stack.h>
#include <WiFi.h>
#include <time.h>
#include "ncmb.h"

const char wifiSsid[] = "rs500k-3767e1-1";
const char wifiPassphrase[] = "18663ffc77916";
const char ncmbApplicationKey[] = "fa18b8060f2df50b1e200a8d7021a32d5cb9d6958c8a7bfc20dfce354d38a0db";
const char ncmbClientKey[] = "41152ee29a51bfc4fd728d60d96b339a223c13a854b431fb6b53957bcb76fdd3";

NCMB ncmb; // mobile backed アクセス用

void setup() {
  // ディスプレイ
  M5.begin();
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 0);

  // WiFi
  WiFi.begin(wifiSsid, wifiPassphrase);
  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
  }

  // 時計機能（nict より時刻取得）
  configTime(0, 0, "ntp.nict.jp");

  // mobile backend
  ncmb.init(ncmbApplicationKey, ncmbClientKey);
}

void loop() {
  M5.update();
  if (M5.BtnA.wasPressed()) {
    registerHistory("ohayo");
  }
  if(M5.BtnC.wasPressed()) {
    registerHistory("oyasumi");
  }
  delay(50);
}

void registerHistory(String item) {
  // 現在時刻の取得
  struct tm timeInfo;
  getLocalTime(&timeInfo);
  String dateString = getDateString(&timeInfo);
  String timeString = getTimeString(&timeInfo);
  String timestamp = getTimestamp(&timeInfo);

  // ディスプレイへ表示
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println(timeString + " " + item);

  // mobile backend への登録
  String content = "{\"item\":\"" + item + "\",\"date\":\"" + dateString + "\"}";
  NCMBResponse response = ncmb.registerObject("history", content, timestamp);
  M5.Lcd.println((response.isSuccess) ? "registration is succeeded" : "registration is failed");
}

// 引数より "YYYY-MM-DD" 形式の日付文字列を返す
String getDateString(struct tm *timeInfo) {
  String date = "20YY-MM-DD";
  date.setCharAt(2, ((0 < (timeInfo->tm_year - 100) / 10) ? (timeInfo->tm_year - 100) / 10 : 0) + 48);
  date.setCharAt(3, (timeInfo->tm_year - 100) % 10 + 48);
  date.setCharAt(5, ((0 < (timeInfo->tm_mon + 1) / 10) ? (timeInfo->tm_mon + 1) / 10 : 0) + 48);
  date.setCharAt(6, (timeInfo->tm_mon + 1) % 10 + 48);
  date.setCharAt(8, ((0 < timeInfo->tm_mday / 10) ? timeInfo->tm_mday / 10 : 0) + 48);
  date.setCharAt(9, timeInfo->tm_mday % 10 + 48);
  return date;  
}

// 引数より "HH:mm:ss.000Z" 形式の時刻文字列を返す
String getTimeString(struct tm *timeInfo) {
  String time = "HH:mm:ss.000Z";
  time.setCharAt(0, ((0 < timeInfo->tm_hour / 10) ? timeInfo->tm_hour / 10 : 0) + 48);
  time.setCharAt(1, timeInfo->tm_hour % 10 + 48);
  time.setCharAt(3, ((0 < timeInfo->tm_min / 10) ? timeInfo->tm_min / 10 : 0) + 48);
  time.setCharAt(4, timeInfo->tm_min % 10 + 48);
  time.setCharAt(6, ((0 < timeInfo->tm_sec / 10) ? timeInfo->tm_sec / 10 : 0) + 48);
  time.setCharAt(7, timeInfo->tm_sec % 10 + 48);
  return time;  
}

// 引数よりISO形式 "YYYY-MM-DDTHH:mm:ss.000Z" の日時文字列を返す
String getTimestamp(struct tm *timeInfo) {
  String timestamp = getDateString(timeInfo);
  timestamp.concat("T");
  timestamp.concat(getTimeString(timeInfo));
  return timestamp;
}
