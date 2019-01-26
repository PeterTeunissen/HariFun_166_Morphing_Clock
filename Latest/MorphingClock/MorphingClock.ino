// Morphing Clock by Hari Wiguna, July 2018
//
// Thanks to:
// Dominic Buchstaller for PxMatrix
// Tzapu for WifiManager
// Stephen Denne aka Datacute for DoubleResetDetector
// Brian Lough aka WitnessMeNow for tutorials on the matrix and WifiManager

#define double_buffer
#include <PxMatrix.h>
#include "Digit.h"
#include "NTPClient.h"
#include "TinyFont.h"

#ifdef ESP32

#define P_LAT 22
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 5
#define P_E 15
#define P_OE 2
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

#endif

#ifdef ESP8266

#include <Ticker.h>
Ticker display_ticker;
#define P_LAT 16
#define P_A 5
#define P_B 4
#define P_C 15
#define P_D 12
#define P_E 0
#define P_OE 2

#endif

// Pins for LED MATRIX
PxMATRIX display(64, 32, P_LAT, P_OE, P_A, P_B, P_C, P_D, P_E);

#ifdef ESP8266
// ISR for display refresh
void display_updater()
{
  //display.displayTestPattern(70);
  display.display(70);
}
#endif

#ifdef ESP32
void IRAM_ATTR display_updater() {
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  //isplay.display(70);
  display.displayTestPattern(70);
  portEXIT_CRITICAL_ISR(&timerMux);
}
#endif

//=== SEGMENTS ===
Digit digit0(&display, 0, 63 - 1 - 9 * 1, 8, display.color565(0, 0, 255));
Digit digit1(&display, 0, 63 - 1 - 9 * 2, 8, display.color565(0, 0, 255));
Digit digit2(&display, 0, 63 - 4 - 9 * 3, 8, display.color565(0, 0, 255));
Digit digit3(&display, 0, 63 - 4 - 9 * 4, 8, display.color565(0, 0, 255));
Digit digit4(&display, 0, 63 - 7 - 9 * 5, 8, display.color565(0, 0, 255));
Digit digit5(&display, 0, 63 - 7 - 9 * 6, 8, display.color565(0, 0, 255));

uint16_t digitColor = display.color565(0, 0, 255);
uint16_t amColor = display.color565(61, 165, 43);
uint16_t pmColor = display.color565(60, 20, 0);
uint16_t colonColor = display.color565(0, 0, 255);

//=== CLOCK ===
NTPClient ntpClient;
unsigned long prevEpoch;
byte prevhh;
byte prevmm;
byte prevss;

void setup() {
  Serial.begin(9600);
  display.setDriverChip(FM6126A);
  display.begin(16);

#ifdef ESP8266
  display_ticker.attach(0.002, display_updater);
#endif

#ifdef ESP32
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &display_updater, true);
  timerAlarmWrite(timer, 2000, true);
  timerAlarmEnable(timer);
#endif

  ntpClient.Setup(&display);

  if (!ntpClient.useMilitary()) {
    digit0.setSize(3);
    digit0.setY(digit0.getY() + 6);
    digit0.setX(digit0.getX() - 1);
    digit1.setSize(3);
    digit1.setX(digit1.getX() + 2);
    digit1.setY(digit1.getY() + 6);
  }
  digit2.setColonLeft(false);
  digit4.setColonLeft(false);
  display.fillScreen(display.color565(0, 0, 0));
  digit2.DrawColon(colonColor);
  digit4.DrawColon(colonColor);
}

bool isAMFlag = true;
bool digit5Hidden = false;

String location = "Phoenixville,US"; //e.g. "Paris,FR"
char server[]   = "api.openweathermap.org";
WiFiClient client;
String apiKey = "aec6c8810510cce7b0ee8deca174c79a";
//http://api.openweathermap.org/data/2.5/weather?q=Phoenixville,PA&appid=aec6c8810510cce7b0ee8deca174c79a&cnt=1&units=metric
int tempMin = -10000;
int tempMax = -10000;
int tempM = -10000;
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
bool timeToParse = false;
bool timeToDrawWeather = false;
byte parseIndex = 0;

void getWeather () {
  if (!apiKey.length ())
  {
    Serial.println ("w:missing API KEY for weather data, skipping");
    return;
  }
  Serial.println ("i:connecting to weather server.. ");
  // if you get a connection, report back via serial:
  if (client.connect (server, 80))
  {
    Serial.println ("connected.");
    // Make a HTTP request:
    client.print ("GET /data/2.5/weather?");
    client.print ("q=" + location);
    client.print ("&appid=" + apiKey);
    client.print ("&cnt=1");
    //    (*u_metric=='Y')?client.println ("&units=metric"):
    client.println ("&units=imperial");
    client.println ("Host: api.openweathermap.org");
    client.println ("Connection: close");
    client.println ();
  } else {
    Serial.println ("w:unable to connect");
    return;
  }
  delay(1000);
  line = "";
  line = client.readStringUntil ('\n');
  if (!line.length ()) {
    Serial.println ("w:unable to retrieve weather data");
  } else {
    timeToParse = true;
    parseIndex = 0;
    Serial.print("got weather. Len:");
    Serial.println(line.length());
  }
}

void parseLoop() {
  if (!timeToParse) {
    return;
  }

  String sval = "";
  int bT, bT2;

  //Serial.println (line);
  //weather conditions - "main":"Clear",
  
  parseIndex++;
  Serial.print("Parse:");
  Serial.println(parseIndex);
  
  switch (parseIndex) {
    case 1:
      bT = line.indexOf ("\"icon\":\"");
      if (bT > 0)
      {
        bT2 = line.indexOf ("\"", bT + 8);
        sval = line.substring (bT + 8, bT2);
        Serial.print ("cond ");
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
        Serial.print ("condM ");
        Serial.println (condM);
      }
      break;
 
    case 2:
      //tempM
      bT = line.indexOf ("\"temp\":");
      if (bT > 0) {
        bT2 = line.indexOf (",\"", bT + 7);
        sval = line.substring (bT + 7, bT2);
        Serial.print ("temp: ");
        Serial.println (sval);
        tempM = sval.toInt ();
      } else {
        Serial.println ("temp NOT found!");
      }
      break;

    case 3:
      //pressM
      bT = line.indexOf ("\"pressure\":");
      if (bT > 0) {
        bT2 = line.indexOf (",\"", bT + 11);
        sval = line.substring (bT + 11, bT2);
        Serial.print ("press ");
        Serial.println (sval);
        presM = sval.toInt();
      } else {
        Serial.println ("pressure NOT found!");
      }
      break;

    case 4:
      //humiM
      bT = line.indexOf ("\"humidity\":");
      if (bT > 0) {
        bT2 = line.indexOf (",\"", bT + 11);
        sval = line.substring (bT + 11, bT2);
        Serial.print ("humi ");
        Serial.println (sval);
        humiM = sval.toInt();
      } else {
        Serial.println ("humidity NOT found!");
      }
      break;

    case 5:
      //gust
      bT = line.indexOf ("\"gust\":");
      if (bT > 0) {
        bT2 = line.indexOf (",\"", bT + 7);
        sval = line.substring (bT + 7, bT2);
        gust = sval.toInt();
      } else {
        Serial.println ("windspeed NOT found!");
        gust = 0;
      }
      break;
    
    case 6:
      //wind speed
      bT = line.indexOf ("\"speed\":");
      if (bT > 0) {
        bT2 = line.indexOf (",\"", bT + 8);
        sval = line.substring (bT + 8, bT2);
        wind_speed = sval.toInt();
      } else {
        Serial.println ("windspeed NOT found!");
      }
      break;
    
    case 7:
      //wind direction
      bT = line.indexOf ("\"deg\":");
      if (bT > 0) {
        bT2 = line.indexOf (",\"", bT + 6);
        sval = line.substring (bT + 6, bT2);
        wind_nr = round(((sval.toInt() % 360)) / 45.0) + 1;
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
      } else {
        Serial.println ("windspeed NOT found!");
        wind_direction = "";
      }
      break;

    case 8:
      timeToParse = false;
      timeToDrawWeather = true;
      break;
  }
}//connected


void draw_weather () {
  if (!timeToDrawWeather) {
    return;
  }
  timeToDrawWeather = false;
  int cc_wht = display.color565 (255, 255, 255);
  int cc_red = display.color565 (255, 0, 0);
  int cc_org = display.color565 (255, 165, 0);
  int cc_grn = display.color565 (0, 255, 0);
  int cc_blu = display.color565 (0, 128, 255);
  int cc_ylw = display.color565 (255, 255, 0);
  int cc_gry = display.color565 (128, 128, 128);
  int cc_dgr = display.color565 (30, 30, 30);
  int cc_lblu = display.color565 (0, 255, 255);
  int cc_ppl = display.color565 (255, 0, 255);
  int value = 0;
  Serial.println ("showing the weather");
  xo = 0;
  yo = 1;
  TFDrawText (&display, String("                   "), xo, yo, cc_dgr);
  if (tempM == -10000 || humiM == -10000 || presM == -10000)
  {
    //TFDrawText (&display, String("NO WEATHER DATA"), xo, yo, cc_dgr);
    Serial.println ("!no weather data available");
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
    String lstr = String (tempM) + String((*u_metric == 'Y') ? "C" : "F");
    Serial.print ("temperature: ");
    Serial.println (lstr);
    TFDrawText (&display, lstr, xo, yo, lcc);
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
    lstr = String (humiM) + "%";
    xo = 6 * TF_COLS;
    TFDrawText (&display, lstr, xo, yo, lcc);
    //-pressure
    lstr = String (presM);
    xo = 12 * TF_COLS;
    if (presM < 1000) {
      xo = 13 * TF_COLS;
    }
    TFDrawText (&display, lstr, xo, yo, cc_gry);
    //draw wind speed and direction
    if (wind_speed > -10000) {
      xo = 0 * TF_COLS;
      yo = 26;
      TFDrawText (&display, "   ", xo, yo, 0);
      //if there is gust, draw gust instead of wind speed
      if (gust > wind_speed) {
        value = gust;
      } else {
        value = wind_speed;
      }
      //if there is gust, draw gust instead of wind speed
      lstr = String (value) + String((gust > wind_speed) ? "'" : "");
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
      Serial.print ("wind_speed: ");
      Serial.println (lstr);
      TFDrawText (&display, lstr, xo, yo, ct);
    }
    if (wind_direction)
    {
      xo = 14 * TF_COLS;
      yo = 26;
      TFDrawText (&display, "   ", xo, yo, 0);
      if (wind_direction.length() == 1) {
        xo = 15 * TF_COLS;
      }

      lstr = String (wind_direction);

      Serial.print ("wind_direction: ");
      Serial.println (lstr);
      TFDrawText (&display, lstr, xo, yo, cc_gry);
    }

    //weather conditions
    //draw_weather_conditions ();
  }
}

void updateDate() {
  char dstring[15];
  sprintf(dstring, "%d/%d/%d", ntpClient.getMonth(), ntpClient.getDay(), ntpClient.getYear());
  String txt = String(dstring);
  TFDrawText(&display, txt, 12, 26, display.color565(51, 0, 26));
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
      digit0.Draw(ss % 10);
      digit1.Draw(ss / 10);
      digit2.Draw(mm % 10);
      digit3.Draw(mm / 10);
      digit4.Draw(hh % 10);
      digit5.Draw(hh / 10);

      updateDate();
      getWeather();
      draw_weather();

      if (!ntpClient.useMilitary()) {
        isAMFlag = ntpClient.isAM();
        if (isAMFlag) {
          TFDrawChar(&display, 'A', 63 - 1 + 3 - 9 * 2, 19, amColor);
          TFDrawChar(&display, 'M', 63 - 1 - 2 - 9 * 1, 19, amColor);
        } else {
          TFDrawChar(&display, 'P', 63 - 1 + 3 - 9 * 2, 19, pmColor);
          TFDrawChar(&display, 'M', 63 - 1 - 2 - 9 * 1, 19, pmColor);
        }
      }
    }
    else
    {
      if (!ntpClient.useMilitary() && (ntpClient.isAM() != isAMFlag)) {
        isAMFlag = ntpClient.isAM();
        if (isAMFlag) {
          TFDrawChar(&display, 'A', 63 - 1 + 3 - 9 * 2, 19, amColor);
          TFDrawChar(&display, 'M', 63 - 1 - 2 - 9 * 1, 19, amColor);
        } else {
          TFDrawChar(&display, 'P', 63 - 1 + 3 - 9 * 2, 19, pmColor);
          TFDrawChar(&display, 'M', 63 - 1 - 2 - 9 * 1, 19, pmColor);
        }
      }

      // epoch changes every miliseconds, we only want to draw when digits actually change.
      if (ss != prevss) {
      parseLoop();
      draw_weather();
        int s0 = ss % 10;
        int s1 = ss / 10;
        if (s0 != digit0.Value()) {
          digit0.Morph(s0);
        }
        if (s1 != digit1.Value()) {
          digit1.Morph(s1);
        }
        //ntpClient.PrintTime();
        prevss = ss;
        //refresh weather every 5mins at 30sec in the minute
        if (ss == 30 && ((mm % 5) == 0)) {
          getWeather ();
        }
      }

      if (mm != prevmm) {
        int m0 = mm % 10;
        int m1 = mm / 10;
        if (m0 != digit2.Value()) {
          digit2.Morph(m0);
        }
        if (m1 != digit3.Value()) {
          digit3.Morph(m1);
        }
        prevmm = mm;
      }

      if (hh != prevhh) {
        updateDate();
        int h0 = hh % 10;
        int h1 = hh / 10;
        if (h0 != digit4.Value()) {
          digit4.Morph(h0);
        }
        if (ntpClient.useMilitary()) {
          if (h1 != digit5.Value()) {
            digit5.Morph(h1);
          }
        } else {
          if (h1 == 0 && (!digit5Hidden)) {
            digit5.hide();
            digit5Hidden = true;
          } else {
            if (h1 != digit5.Value()) {
              digit5Hidden = false;
              digit5.Morph(h1);
            }
          }
        }
        prevhh = hh;
      }
    }
    prevEpoch = epoch;
  }
}
