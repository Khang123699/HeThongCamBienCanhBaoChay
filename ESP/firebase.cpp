#include "firebase.h"
#include "data.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <time.h>

#define DATABASE_URL "https://hethongcanhbaochay-46db4-default-rtdb.asia-southeast1.firebasedatabase.app"

static bool timeSynced = false;
static unsigned long lastSend = 0;
#define MIN_SEND_INTERVAL 500 // chống spam

void syncTime() {
  configTime(7 * 3600, 0, "pool.ntp.org");
}

bool isTimeSynced() {
  return time(nullptr) > 100000;
}

String getTimeString() {
  time_t now = time(nullptr);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  char buf[32];
  strftime(buf, sizeof(buf), "%H:%M:%S", &timeinfo);
  return String(buf);
}

void FirebaseApp_Init(void) {
  syncTime();
}

void FirebaseApp_Process(void) {
  if (WiFi.status() != WL_CONNECTED) return;
  if (!isTimeSynced()) return;

  // chỉ gửi khi có thay đổi
  if (!dataChanged) return;

  // chống spam
  if (millis() - lastSend < MIN_SEND_INTERVAL) return;

  dataChanged = false;
  lastSend = millis();

  FirebaseApp_Send(true);
}

void FirebaseApp_Send(bool force) {

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;

  String json = "{";
  json += "\"state\":\"" + trangThai + "\",";
  json += "\"mq2\":" + String(mq2) + ",";
  json += "\"temp\":" + String(temp) + ",";
  json += "\"hum\":" + String(hum) + ",";
  json += "\"fire\":" + String(fire);
  json += "}";

  // PATCH
  String url = String(DATABASE_URL) + "/sensor.json";
  if (http.begin(client, url)) {
    http.PATCH(json);
    http.end();
  }

  // LOG nếu cháy
  if (fire == 1) {
    url = String(DATABASE_URL) + "/history.json";
    if (http.begin(client, url)) {
      http.POST(json);
      http.end();
    }
  }
}