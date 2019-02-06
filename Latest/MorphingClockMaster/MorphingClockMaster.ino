// Morphing Clock by Hari Wiguna, July 2018
//
// Thanks to:
// Dominic Buchstaller for PxMatrix
// Tzapu for WifiManager
// Stephen Denne aka Datacute for DoubleResetDetector
// Brian Lough aka WitnessMeNow for tutorials on the matrix and WifiManager

#define double_buffer
#include "NTPClient.h"
#include <ArduinoJson.h>
#include <TimeLib.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266HTTPClient.h>

#define ONE_WIRE_BUS_PIN 14

inline uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

#define TF_COLS 4
#define TF_ROWS 5

uint16_t digitColor = color565(0, 0, 255);
uint16_t amColor = color565(61, 165, 43);
uint16_t pmColor = color565(60, 20, 0);
uint16_t colonColor = color565(0, 0, 255);

uint16_t cc_wht = color565 (128, 128, 128);
uint16_t cc_red = color565 (128, 0, 0);
uint16_t cc_org = color565 (255, 165, 0);
uint16_t cc_grn = color565 (0, 128, 0);
uint16_t cc_blu = color565 (0, 128, 255);
uint16_t cc_ylw = color565 (255, 255, 0);
uint16_t cc_gry = color565 (128, 128, 128);
uint16_t cc_dgr = color565 (30, 30, 30);
uint16_t cc_lblu = color565 (0, 255, 255);
uint16_t cc_ppl = color565 (255, 0, 255);

//=== CLOCK ===
NTPClient ntpClient;
unsigned long prevEpoch;
byte prevhh;
byte prevmm;
byte prevss;

OneWire oneWire(ONE_WIRE_BUS_PIN);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);
  Serial.println("Setup done");
  ntpClient.Setup();
  sensors.begin();
}

String StringPieceAsString(const String &line, const String &word, const String &term, byte size) {
  int bT = line.indexOf(word);
  if (bT > 0) {
    int bT2 = line.indexOf (term, bT + size);
    return line.substring (bT + size, bT2);
  }
  return "";
}

int StringPieceAsInt(const String &line, const String &word, const String &term, byte size) {
  return StringPieceAsString(line, word, term, size).toInt();
}

bool isAMFlag = true;
bool digit5Hidden = false;

//String location = "Phoenixville,US"; //e.g. "Paris,FR"
char server[]   = "api.openweathermap.org";
WiFiClient client;
String apiKey = "aec6c8810510cce7b0ee8deca174c79a";
//http://api.openweathermap.org/data/2.5/weather?q=Phoenixville,PA&appid=aec6c8810510cce7b0ee8deca174c79a&cnt=1&units=metric
//int tempMin = -10000;
//int tempMax = -10000;
int tempM = -10000;
int tempTMP = 0;
int prevTempTMP = 0;
float f1=0.0;
float f2=0.0;
float f3=0.0;
int presM = -10000;
int humiM = -10000;
int condM = -1;  //-1 - undefined, 0 - unk, 1 - sunny, 2 - cloudy, 3 - overcast, 4 - rainy, 5 - thunders, 6 - snow
String condS = "";
int wind_speed;
int wind_nr;
String wind_direction = "";
int gust = 0;
int xo;
int yo;
char u_metric[] = "N";
String line;
bool parseLine = false;
byte parseIndex = 0;

char serverTZ[]  = "api.timezonedb.com";
String tzKey = "31VLCCL5BAKD";
String tzZone = "America/New_York";
bool haveTZ = false;
char tzLine[100];

void getTimeZone() {
  HTTPClient http;
  String url = "http://api.timezonedb.com/v2.1/get-time-zone?key=" + tzKey + "&format=json&by=zone&zone=" + tzZone + "&fields=gmtOffset,timestamp";
  http.begin(url);
  int httpCode = http.GET();  
  if (httpCode > 0) { //Check the returning code
    strcpy(tzLine, http.getString().c_str());   //Get the request response payload
    Serial.print("HTTPTZ Resp:");                     //Print the response payload
    Serial.println(tzLine);                     //Print the response payload
    StaticJsonBuffer<200> jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(tzLine);  
    if (!json.success()) {
      Serial.println("Failed to parse json string");
    } else {
      char b[10];
      Serial.print("From json: timestamp:");
      strcpy(b,json["timestamp"]);
      int tm = atoi(json["timestamp"]);
      setTime(tm);
      Serial.print(b);
      Serial.print(" gmtOffSet:");
      strcpy(b,json["gmtOffset"]);
      Serial.print(b);
      Serial.println();
    }  
  }  
  http.end();   
}

//http://api.openweathermap.org/data/2.5/weather?q=Phoenixville,PA&appid=aec6c8810510cce7b0ee8deca174c79a&cnt=1&units=metric
void getWeather () {
  if (!apiKey.length ())
  {
    Serial.println (F("w:missing API KEY for weather data, skipping"));
    return;
  }
  Serial.println (F("i:connecting to weather server.. "));
  if (client.connect (server, 80))
  {
    Serial.println (F("connected."));
    // Make a HTTP request:
    client.print ("GET /data/2.5/weather?");
    client.print (String("q=") + String("Phoenixville,US"));  // location
    client.print ("&appid=" + apiKey);
    client.print ("&cnt=1");
    //    (*u_metric=='Y')?client.println ("&units=metric"):
    client.println ("&units=imperial");
    client.println (F("Host: api.openweathermap.org"));
    client.println (F("Connection: close"));
    client.println ();
  } else {
    Serial.println (F("w:unable to connect"));
    return;
  }
  String sval = "";
  delay(1000);
  line = client.readStringUntil('\n');
  client.stop(100);
  if (!line.length ())
    Serial.println (F("w:unable to retrieve weather data"));
  else
  {
    Serial.print (F("weather:"));
    Serial.println (line);
    parseLine = true;
    parseIndex = 0;
    //weather conditions - "main":"Clear",
    sval = StringPieceAsString(line, "\"icon\":\"", "\"", 8);
    //    bT = line.indexOf ("\"icon\":\"");
    //    if (bT > 0)
    //    {
    //      bT2 = line.indexOf ("\"", bT + 8);
    //      sval = line.substring (bT + 8, bT2);
    Serial.print (F("cond "));
    Serial.println (sval);
    //0 - unk, 1 - sunny, 2 - cloudy, 3 - overcast, 4 - rainy, 5 - thunders, 6 - snow
    if (sval.equals("01d"))
      condM = 1; //sunny
    else if (sval.equals("01n"))
      condM = 8; //clear night
    else if (sval.equals("02d"))
      condM = 2; //partly cloudy day
    else if (sval.equals("02n"))
      condM = 10; //partly cloudy night
    else if (sval.equals("03d"))
      condM = 3; //overcast day
    else if (sval.equals("03n"))
      condM = 11; //overcast night
    else if (sval.equals("04d"))
      condM = 3;//overcast day
    else if (sval.equals("04n"))
      condM = 11;//overcast night
    else if (sval.equals("09d"))
      condM = 4; //rain
    else if (sval.equals("09n"))
      condM = 4;
    else if (sval.equals("10d"))
      condM = 4;
    else if (sval.equals("10n"))
      condM = 4;
    else if (sval.equals("11d"))
      condM = 5; //thunder
    else if (sval.equals("11n"))
      condM = 5;
    else if (sval.equals("13d"))
      condM = 6; //snow
    else if (sval.equals("13n"))
      condM = 6;
    else if (sval.equals("50d"))
      condM = 7; //haze (day)
    else if (sval.equals("50n"))
      condM = 9; //fog (night)
    //
    condS = sval;
    Serial.print (F("condM "));
    Serial.println (condM);
    //    }
    //tempM
    //    bT = line.indexOf ("\"temp\":");
    //    if (bT > 0) {
    //      bT2 = line.indexOf (",\"", bT + 7);
    //      sval = line.substring (bT + 7, bT2);
    //      Serial.print ("temp: ");
    //      Serial.println (sval);
    //      tempM = sval.toInt ();
    //    } else {
    //      Serial.println ("temp NOT found!");
    //    }
    tempM = StringPieceAsInt(line, "\"temp\":", ",\"", 7);
    Serial.print ("temp: ");
    Serial.println (tempM);
    //pressM
    //    bT = line.indexOf ("\"pressure\":");
    //    if (bT > 0) {
    //      bT2 = line.indexOf (",\"", bT + 11);
    //      sval = line.substring (bT + 11, bT2);
    //      Serial.print ("press ");
    //      Serial.println (sval);
    //      presM = sval.toInt();
    //    } else {
    //      Serial.println ("pressure NOT found!");
    //    }
    presM = StringPieceAsInt(line, "\"pressure\":", ",\"", 11);
    Serial.print ("press ");
    Serial.println (presM);
    //humiM
    //    bT = line.indexOf ("\"humidity\":");
    //    if (bT > 0) {
    //      bT2 = line.indexOf (",\"", bT + 11);
    //      sval = line.substring (bT + 11, bT2);
    //      Serial.print ("humi ");
    //      Serial.println (sval);
    //      humiM = sval.toInt();
    //    } else {
    //      Serial.println ("humidity NOT found!");
    //    }
    humiM = StringPieceAsInt(line, "\"humidity\":", ",\"", 11);
    Serial.print ("humi ");
    Serial.println (humiM);
    //gust
    //    bT = line.indexOf ("\"gust\":");
    //    if (bT > 0) {
    //      bT2 = line.indexOf (",\"", bT + 7);
    //      sval = line.substring (bT + 7, bT2);
    //      gust = sval.toInt();
    //    } else {
    //      Serial.println ("windspeed NOT found!");
    //      gust = 0;
    //    }
    gust = StringPieceAsInt(line, "\"gust\":", ",\"", 7);
    Serial.print ("gust ");
    Serial.println (gust);
    //wind speed
    //    bT = line.indexOf ("\"speed\":");
    //    if (bT > 0) {
    //      bT2 = line.indexOf (",\"", bT + 8);
    //      sval = line.substring (bT + 8, bT2);
    //      wind_speed = sval.toInt();
    //    } else {
    //      Serial.println ("windspeed NOT found!");
    //    }
    wind_speed = StringPieceAsInt(line, "\"speed\":", ",\"", 8);
    Serial.print ("windspeed ");
    Serial.println (wind_speed);
    //wind direction
    wind_nr = StringPieceAsInt(line, "\"deg\":", ",\"", 6);
    wind_nr = round(((wind_nr % 360)) / 45.0) + 1;
    //    bT = line.indexOf ("\"deg\":");
    //    if (bT > 0) {
    //      bT2 = line.indexOf (",\"", bT + 6);
    //      sval = line.substring (bT + 6, bT2);
    //      wind_nr = round(((sval.toInt() % 360))/45.0) + 1;
    wind_direction = "";
    switch (wind_nr) {
      case 1:
        wind_direction = "N";
        break;
      case 2:
        wind_direction = "NE";
        break;
      case 3:
        wind_direction = "E";
        break;
      case 4:
        wind_direction = "SE";
        break;
      case 5:
        wind_direction = "S";
        break;
      case 6:
        wind_direction = "SW";
        break;
      case 7:
        wind_direction = "W";
        break;
      case 8:
        wind_direction = "NW";
        break;
      case 9:
        wind_direction = "N";
        break;
      default:
        wind_direction = "";
        break;
    }
    Serial.print ("wind direction ");
    Serial.println(wind_direction);
    //    } else {
    //      Serial.println ("windspeed NOT found!");
    //      wind_direction = "";
    //    }
  }//connected
}

void drawWeather () {
  if (prevEpoch == 0) {
    return;
  }
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  
  // Add:
  // Digit color
  
  json["tmpU"] = String((*u_metric == 'Y') ? "C" : "F");
  json["mil"] = String((ntpClient.useMilitary()?"Y":"N"));
  json["bar"] = presM;
  json["barC"] = cc_gry;
  
//  json["wnd"] = wind_speed; 
//  json["gst"] = gust;
  json["dir"] = wind_direction;
  json["tim"] = ntpClient.GetCurrentTime();

  int value = 0;
  if (tempM == -10000 || humiM == -10000 || presM == -10000)
  {
    Serial.println (F("!no weather data available"));
  } else {
    //weather below the clock
    //-temperature
    int lcc = cc_red;
    if (*u_metric == 'Y') {
      if (tempM >= 30)
        lcc = cc_red;
      if (tempM >= 25)
        lcc = cc_org;
      if (tempM < 25)
        lcc = cc_ylw;
      if (tempM < 20)
        lcc = cc_grn;
      if (tempM < 15)
        lcc = cc_blu;
      if (tempM < 10)
        lcc = cc_lblu;
      if (tempM < 1)
        lcc = cc_wht;
    } else {
      //F
      if (tempM < 79)
        lcc = cc_grn;
      if (tempM < 64)
        lcc = cc_blu;
      if (tempM < 43)
        lcc = cc_wht;
    }
    json["otmp"] = tempM;
    json["otmpC"] = lcc;

    lcc = cc_red;
    if (*u_metric == 'Y') {
      if (tempTMP >= 30)
        lcc = cc_red;
      if (tempTMP >= 25)
        lcc = cc_org;
      if (tempTMP < 25)
        lcc = cc_ylw;
      if (tempTMP < 20)
        lcc = cc_grn;
      if (tempTMP < 15)
        lcc = cc_blu;
      if (tempTMP < 10)
        lcc = cc_lblu;
      if (tempTMP < 1)
        lcc = cc_wht;
    } else {
      //F
      if (tempTMP < 79)
        lcc = cc_grn;
      if (tempTMP < 64)
        lcc = cc_blu;
      if (tempTMP < 43)
        lcc = cc_wht;
    }    
    json["itmp"] = tempTMP;
    json["itmpC"] = lcc;

    //weather conditions
    //-humidity
    lcc = cc_red;
    if (humiM < 80)
      lcc = cc_org;
    if (humiM < 60)
      lcc = cc_grn;
    if (humiM < 40)
      lcc = cc_blu;
    if (humiM < 20)
      lcc = cc_wht;
    json["hum"] = String (humiM) + "%";
    json["humC"] = lcc;
    //-pressure
    //draw wind speed and direction
    if (wind_speed > -10000) {
      //if there is gust, draw gust instead of wind speed
      if (gust > wind_speed) {
        value = gust;
      } else {
        value = wind_speed;
      }
      //if there is gust, draw gust instead of wind speed
      String lstr = String (value) + String((gust > wind_speed) ? "'" : "");
      json["wnd"] = lstr; 
      int ct = cc_wht;
      if (value >= 1) {
        ct = cc_grn;
      }
      if (value >= 4) {
        ct = cc_lblu;
      }
      if (value >= 8) {
        ct = cc_blu;
      }
      if (value >= 12) {
        ct = cc_ylw;
      }
      if (value >= 16) {
        ct = cc_org;
      }
      if (value >= 20) {
        ct = cc_red;
      }
      json["wndC"] = ct;
    }
  }
  json.printTo(Serial);
  Serial.println();
}

void loop() {
  unsigned long epoch = ntpClient.GetCurrentTime();

  //Serial.print("GetCurrentTime returned epoch = ");
  //Serial.println(epoch);
  if (epoch != 0) {
    ntpClient.PrintTime();
  }

  if (epoch != prevEpoch) {
    int hh = ntpClient.GetHours();
    int mm = ntpClient.GetMinutes();
    int ss = ntpClient.GetSeconds();
    if (prevEpoch == 0) { // If we didn't have a previous time. Just draw it without morphing.
//      digit0.Draw(ss % 10);
//      digit1.Draw(ss / 10);
//      digit2.Draw(mm % 10);
//      digit3.Draw(mm / 10);
//      digit4.Draw(hh % 10);
//      digit5.Draw(hh / 10);

      getTimeZone();
      getWeather();
      drawWeather();

      if (!ntpClient.useMilitary()) {
        isAMFlag = ntpClient.isAM();
        if (isAMFlag) {
  //        TFDrawChar(&display, 'A', 63 - 1 + 3 - 9 * 2, 19, amColor);
  //        TFDrawChar(&display, 'M', 63 - 1 - 2 - 9 * 1, 19, amColor);
        } else {
  //        TFDrawChar(&display, 'P', 63 - 1 + 3 - 9 * 2, 19, pmColor);
  //        TFDrawChar(&display, 'M', 63 - 1 - 2 - 9 * 1, 19, pmColor);
        }
      }
    }
    else
    {
      if (!ntpClient.useMilitary() && (ntpClient.isAM() != isAMFlag)) {
        isAMFlag = ntpClient.isAM();
        if (isAMFlag) {
  //        TFDrawChar(&display, 'A', 63 - 1 + 3 - 9 * 2, 19, amColor);
  //        TFDrawChar(&display, 'M', 63 - 1 - 2 - 9 * 1, 19, amColor);
        } else {
  //        TFDrawChar(&display, 'P', 63 - 1 + 3 - 9 * 2, 19, pmColor);
  //        TFDrawChar(&display, 'M', 63 - 1 - 2 - 9 * 1, 19, pmColor);
        }
      }

      // epoch changes every miliseconds, we only want to draw when digits actually change.
      if (ss != prevss) {
        int s0 = ss % 10;
        int s1 = ss / 10;
//        if (s0 != digit0.Value()) {
  //        digit0.Morph(s0);
//        }
 //       if (s1 != digit1.Value()) {
  //        digit1.Morph(s1);
 //       }
        //ntpClient.PrintTime();
        prevss = ss;
        //refresh weather every 5mins at 30sec in the minute
        if (ss == 30 && ((mm % 5) == 0)) {
          getWeather ();
        }
        if ((ss == 15) || (ss == 45)) {
          f1 = analogRead(A0);
          f2 = ((f1 * (3.2)) - 500) / 10;
          f3 = (f2 * 1.8) + 32;
          tempTMP = round(f3);
          if (tempTMP!=prevTempTMP) {
            drawWeather();
          }
          prevTempTMP = tempTMP;
        }
      }

      if (mm != prevmm) {
        int m0 = mm % 10;
        int m1 = mm / 10;
//        if (m0 != digit2.Value()) {
//          digit2.Morph(m0);
//        }
//        if (m1 != digit3.Value()) {
//          digit3.Morph(m1);
//        }
        prevmm = mm;
        drawWeather();
      }

      if (hh != prevhh) {
        int h0 = hh % 10;
        int h1 = hh / 10;
//        if (h0 != digit4.Value()) {
   //       digit4.Morph(h0);
  //      }
   //     if (ntpClient.useMilitary()) {
   //       if (h1 != digit5.Value()) {
   //         digit5.Morph(h1);
   //       }
    //    } else {
   //       if (h1 == 0 && (!digit5Hidden)) {
   //         digit5.hide();
   //         digit5Hidden = true;
    //      } else {
   //         if (h1 != digit5.Value()) {
    //          digit5Hidden = false;
   //           digit5.Morph(h1);
   //         }
//          }
 //       }
        prevhh = hh;
      }
    }
    prevEpoch = epoch;
  }
}
