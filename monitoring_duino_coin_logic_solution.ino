#include <ESP8266WiFi.h>
#include <Wire.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h> // V6!
//#include "SH1106Wire.h"
#include "SSD1306Wire.h"

const char* ssid = "ganti dengan nama wifianda";
const char* password = "ganti password wifi anda";
const String ducoUser = "ganti username duino kalian"; // Change this to your Duino-Coin username
const String ducoReportJsonUrl = "https://server.duinocoin.com/v2/users/" + ducoUser + "?limit=1";
const int run_in_ms = 5000;
float lastAverageHash = 0.0;
float lastTotalHash = 0.0;

//SH1106Wire display(0x3c, D2, D1);
SSD1306Wire  display(0x3c, 5, 4);

void setup() {
 unsigned short count = 0; 
 
 Serial.begin(115200);
 
 Serial.println();
 
 Serial.print("Connecting to ");
 Serial.println(ssid);
 
 WiFi.begin(ssid, password);

 while (WiFi.status() != WL_CONNECTED) {
   delay(500);
   Serial.print(".");
   //count++;

   //if (count >= 30)
      //wifiRestart();
 }
 
 Serial.println("");
 Serial.println("WiFi connected");
 Serial.println("IP address: ");
 Serial.println(WiFi.localIP());
 
 display.init();
  display.flipScreenVertically(); 
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  //display.setTextColor(BLACK&WHITE); 
  display.drawString(0,0,"Duino Coin");
  //display.setColor(WHITE);
    //display.drawCircle(32, 32, i*3);
 
  display.drawString(10,25,"Logic Solution");
  display.drawString(0, 40, "Copyright © 2022");
   display.display();
    delay(2000);
   display.clear();

  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawStringMaxWidth(0, 0, 128,
  " 'Mining esp8266, Alat ini hanya untuk monitoring hasil dari mining Duino Coin." );
  display.display();
  delay(5000);
    
    display.clear();



 
}




void loop() {
  if (runEvery(run_in_ms)) {

    float totalHashrate = 0.0;
    float avgHashrate = 0.0;
    int total_miner = 0;

    String input = httpGetString(ducoReportJsonUrl);

    DynamicJsonDocument doc (5000);
    DeserializationError error = deserializeJson(doc, input);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    JsonObject result = doc["result"];
    JsonObject result_balance = result["balance"];
    double result_balance_balance = result_balance["balance"];
    const char *result_balance_created = result_balance["created"];
    const char *result_balance_username = result_balance["username"];
    const char *result_balance_verified = result_balance["verified"];

    for (JsonObject result_miner : result["miners"].as<JsonArray>()) {
      float result_miner_hashrate = result_miner["hashrate"];
      totalHashrate = totalHashrate + result_miner_hashrate;
      total_miner++;

    }

    avgHashrate = totalHashrate / long(total_miner);
    long run_span = run_in_ms / 1000;

 Serial.println("result_balance_username : " + String(result_balance_username));
    Serial.println("result_balance_balance : " + String(result_balance_balance));
    Serial.println("totalHashrate : " + String(totalHashrate));
    Serial.println("avgHashrate H/s : " + String(avgHashrate));
    Serial.println("total_miner : " + String(total_miner));

    display.clear();
    //display.setTextSize(1);
    display.setFont(ArialMT_Plain_10);
    //display.setTextColor(WHITE);
    //display.setCursor(0, 0);
    display.drawString(30, 0, "DUCO Monitor");
    display.drawString(0, 5, " ");
    display.drawString(0, 15, "User: "+ String(result_balance_username));
    display.drawString(0, 25, "Miner   : " + String(total_miner));
    display.drawString(0, 35, "Balance : " + String(result_balance_balance));
    display.drawString(0, 45, "H/s (" + String(run_span) + "s)  :  " + String(totalHashrate));
    //display.setFont(ArialMT_Plain_5);
    
    display.display();
  }
}
String httpGetString(String URL) {

  String payload = "";
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  if (http.begin(client, URL)) {
    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
      payload = http.getString();
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  return payload;
}


boolean runEvery(unsigned long interval) {
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}

 
