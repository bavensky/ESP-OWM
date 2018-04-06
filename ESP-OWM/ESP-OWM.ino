/*
    Open Weather Map using esp8266 to get data

    COLOR                   STATE
    (Very Pale) Blue       Thunderstorm
    (Pale) Blue            Drizzle
    Cyan                   Rain
    White                  Snow
    Gray                   Atmospheric (fog, pollution)
    Yellow                 Clear
    (Pale) Yellow          Clouds / Calm
    Red                    Extreme (tornado, lightning)

    "id": 1153670,
    "name": "Chiang Mai Province",
    "country": "TH",
    "lon": 99,
    "lat": 18.75

 * */



#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>


#define PIN            14
#define NUMPIXELS      16
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ400);


WiFiClient client;

//const char* WIFI_SSID       =  "CMMC_Sinet_2.4G";
//const char* WIFI_PASSWORD   =  "zxc12345";
const char* WIFI_SSID       =  "ClickExec-IAP";
const char* WIFI_PASSWORD   =  "click2499";
String OWM_API_KEY          =  "bd2467167550fefed4dfcafae271f150";
String OWM_CITY_ID          =  "1153670";
String LATITUDE             =  "18.75";
String LONGITUDE            =  "99";
String TIMEZONE             =  "7";
String OWM_UNITS            = "metric";

unsigned long previousMillis = 0;
const long interval = 15000;
int state = 0;
int weather_state = 0;


void checkOWM() {
  StaticJsonBuffer<1000> jsonBuffer;      // JSON Parser setup

  HTTPClient http;
  http.begin("http://api.openweathermap.org/data/2.5/weather?id=" + OWM_CITY_ID + "&appid=" + OWM_API_KEY
             + "&lat=" + LATITUDE  + "&lon=" + LONGITUDE + "&units=" + OWM_UNITS);

  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);

      JsonObject& owm_data = jsonBuffer.parseObject(payload);
      if (!owm_data.success()) {
        Serial.println("Parsing failed");
        return;
      }

      //      String weatherForecast = owm_data["weather"][0]["description"];
      //      Serial.print("weatherForecast = ");
      //      Serial.print(weatherForecast);

      int code = owm_data["weather"][0]["id"];
      weather_state = codeToState(code);
      Serial.print("\tcode = ");
      Serial.print(code);
      Serial.print("\tweatherState = ");
      Serial.print(weather_state);
      Serial.print("\n");
      Serial.print("\n");

      for (int i = 0; i < NUMPIXELS; i++) {
        if (weather_state == 0) {
          pixels.setPixelColor(i, pixels.Color(128, 255, 255));
        } else if (weather_state == 1) {
          pixels.setPixelColor(i, pixels.Color(0, 255, 255));
        } else if (weather_state == 2) {
          pixels.setPixelColor(i, pixels.Color(0, 0, 255));
        } else if (weather_state == 3) {
          pixels.setPixelColor(i, pixels.Color(255, 255, 255));
        } else if (weather_state == 4) {
          pixels.setPixelColor(i, pixels.Color(128, 128, 128));
        } else if (weather_state == 5) {
          pixels.setPixelColor(i, pixels.Color(255, 255, 0));
        } else if (weather_state == 6) {
          pixels.setPixelColor(i, pixels.Color(255, 255, 0));
        } else if (weather_state == 7) {
          pixels.setPixelColor(i, pixels.Color(255, 0, 0));
        }
        pixels.show();
      }
    }
  }
  else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

byte codeToState(uint16_t code) {
  state = 0;
  if (code >= 200 && code < 300) {
    state = 0;    //  Thunderstorm
  }
  else if (code >= 300 && code < 400) {
    state = 1;    //  Drizzle
  }
  else if (code >= 500 && code < 600) {
    state = 2;    //  Rain
  }
  else if (code >= 600 && code < 700) {
    state = 3;    //  Snow
  }
  else if (code >= 700 && code < 800) {
    state = 4;    //  Atmospheric
  }
  else if (code == 800) {
    state = 5;    //  Clear
  }
  else if (code > 800 && code < 900) {
    state = 6;    //  Clouds
  }
  else if (code >= 900 && code < 907) {
    state = 7;    //  Extreme
  }
  return state;
}


void setup() {
  Serial.begin(57600);
  delay(2000);

  if (WiFi.begin(WIFI_SSID, WIFI_PASSWORD)) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  pixels.begin();
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(0, 0, 0));
    pixels.show();
  }
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      checkOWM();
    }
  }
  else  {
    Serial.println("connection lost, reconnect...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    delay(500);
  }
}
