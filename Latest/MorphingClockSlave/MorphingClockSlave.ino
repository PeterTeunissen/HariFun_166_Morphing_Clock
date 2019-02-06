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
#include "TinyFont.h"
#include <TimeLib.h>
#include <ArduinoJson.h>

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

//uint16_t cc_wht = display.color565 (255, 255, 255);
uint16_t cc_wht = display.color565 (128, 128, 128);
//uint16_t cc_red = display.color565 (255, 0, 0);
uint16_t cc_red = display.color565 (128, 0, 0);
uint16_t cc_org = display.color565 (255, 165, 0);
//uint16_t cc_grn = display.color565 (0, 255, 0);
uint16_t cc_grn = display.color565 (0, 128, 0);
uint16_t cc_blu = display.color565 (0, 128, 255);
uint16_t cc_ylw = display.color565 (255, 255, 0);
uint16_t cc_gry = display.color565 (128, 128, 128);
uint16_t cc_dgr = display.color565 (30, 30, 30);
uint16_t cc_lblu = display.color565 (0, 255, 255);
uint16_t cc_ppl = display.color565 (255, 0, 255);

unsigned long prevEpoch;
unsigned long epoch;
bool isReading = false;
bool isParsing = false;
byte parseIndex = 0;
char recBuf[300];
byte prevhh;
byte prevmm;
byte prevss;
bool useMilitary = false;
bool isAMFlag = true;
bool digit5Hidden = false;
int tempM = -10000;
uint16_t tempMC=cc_gry;
int tempTMP = -10000;
uint16_t tempTMPC=cc_gry;
int prevTempTMP = 0;
int presM = -10000;
uint16_t presMC=cc_gry;
int humiM = -10000;
uint16_t humiMC=cc_gry;
int condM = -1;  //-1 - undefined, 0 - unk, 1 - sunny, 2 - cloudy, 3 - overcast, 4 - rainy, 5 - thunders, 6 - snow
String condS = "";
int wind_speed;
int wind_nr;
String wind_direction = "";
uint16_t wndC=cc_gry;
int gust = 0;
int xo;
int yo;
char u_metric[3] = "N";

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

  if (useMilitary) {
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
  strcpy(recBuf,"");
  isParsing = false;
  isReading = true;
  Serial.println("End of Setup");
}

void readLoop() {
  if (isParsing) {
    return;
  }
  
  while (Serial.available()>0) {
    Serial.print(".");
    char c = Serial.read();
    if (c=='>') {
      isReading = true;
      isParsing = false;
      Serial.println(F("Start receive..."));
      strcpy(recBuf,"");
    }
    if (isReading && c>=32 && c<=126 && c!='>' && c!='<') {
      strncat(recBuf,&c,1);
      Serial.print(c);
    }
    if (c=='<') {
      isReading = false;
      isParsing = true;
      parseIndex = 0;
      Serial.println();
      Serial.println(F("Stop receive..."));
      Serial.print(F("Data:"));
      Serial.println(recBuf);
    }
  }
}

void parseLoop() {
  if (!isParsing) {
    return;  
  }

  Serial.print(F("Start parsing..."));
  Serial.println(recBuf);
  
  StaticJsonBuffer<300> jsonBuffer;
  JsonObject& json = jsonBuffer.parseObject(recBuf);

  if (!json.success()) {
    Serial.println("Failed to parse json buf");
    isParsing = false;
    return;
  }

  char b[20];

  tempTMP = atoi(json["itmp"]);
  tempTMPC = atoi(json["itmpC"]);
  tempM = atoi(json["otmp"]);
  tempMC = atoi(json["otmpC"]);
  presM = atoi(json["bar"]);
  presMC = atoi(json["barC"]);
  strcpy(b,json["tmpU"]);
  strcpy(u_metric,b);
  strcpy(b,json["mil"]);
  useMilitary = (strcmp(b,"Y")==0);
  humiM = atoi(json["hum"]);
  humiMC = atoi(json["humC"]);
  strcpy(b,json["dir"]);
  wind_direction = String(b);
  wndC = atoi(json["wndC"]);
  epoch = atoi(json["tim"]);
  setTime(epoch);
  
  isParsing = false;
  Serial.println(F("Done parsing..."));
  strcpy(recBuf,"");
  draw_weather();
}


void draw_weather () {
  int value = 0;
  Serial.println (F("showing the weather"));
  String lstr;
  xo = 0;
  yo = 1;
  TFDrawText (&display, String(F("                   ")), xo, yo, cc_dgr);
  if (tempM == -10000 || humiM == -10000 || presM == -10000)
  {
    //TFDrawText (&display, String("NO WEATHER DATA"), xo, yo, cc_dgr);
    Serial.println (F("!no weather data available"));
  } else {
    lstr = String (tempM) + String((*u_metric == 'Y') ? "C" : "F");
    Serial.print (F("o temp: "));
    Serial.println (lstr);
    TFDrawText (&display, lstr, xo, yo, tempMC);

    xo=xo+(lstr.length()*TF_COLS);
    TFDrawText (&display, String("/"), xo, yo, cc_gry);
    lstr = String(tempTMP) + String((*u_metric == 'Y') ? "C" : "F");
    xo=xo+(1*TF_COLS);
    Serial.print (F("i temp: "));
    Serial.println (lstr);
    TFDrawText (&display, lstr, xo, yo, tempTMPC);
  }  
  
  //weather conditions
  //-humidity
  lstr = String (humiM)+ "%";
  xo = 8 * TF_COLS;
  TFDrawText (&display, lstr, xo, yo, humiMC);
    
  //-pressure
  lstr = String (presM);
  xo = 12 * TF_COLS;
  if (presM < 1000) {
    xo = 13 * TF_COLS;
  }
  TFDrawText (&display, lstr, xo, yo, presMC);

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
    Serial.print (F("wind_speed: "));
    Serial.println (lstr);
    TFDrawText (&display, lstr, xo, yo,wndC );
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

    Serial.print (F("wind_direction: "));
    Serial.println (lstr);
    TFDrawText (&display, lstr, xo, yo, cc_gry);
  }  
}

void updateDate() {
  char dstring[15];
  sprintf(dstring, "%d/%d/%d  ", month(), day(), year());
  String txt = String(dstring);
  TFDrawText(&display, txt, 13, 26, display.color565(51, 0, 26));
}

void loop() {

  readLoop();
  parseLoop();

  epoch = now();
    
  if (epoch != prevEpoch) {
    int hh = hour(epoch); //ntpClient.GetHours();
    int mm = minute(epoch); //ntpClient.GetMinutes();
    int ss = second(epoch); //ntpClient.GetSeconds();
    if (prevEpoch == 0) { // If we didn't have a previous time. Just draw it without morphing.
      digit0.Draw(ss % 10);
      digit1.Draw(ss / 10);
      digit2.Draw(mm % 10);
      digit3.Draw(mm / 10);
      digit4.Draw(hh % 10);
      digit5.Draw(hh / 10);

      updateDate();
      //getWeather();
      draw_weather();

//      if (!ntpClient.useMilitary()) {
      if (!useMilitary) {
       // isAMFlag = ntpClient.isAM();
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
      if (useMilitary && isAMFlag) {
//        isAMFlag = ntpClient.isAM();
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
          //getWeather ();
        }
  //      if ((ss == 15) || (ss == 45)) {
//          tempTMP = round(f3);
 //         if (tempTMP!=prevTempTMP) {
   //         draw_weather();
     //     }
       //   prevTempTMP = tempTMP;
//        }
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
        draw_weather();
      }

      if (hh != prevhh) {
        updateDate();
        int h0 = hh % 10;
        int h1 = hh / 10;
        if (h0 != digit4.Value()) {
          digit4.Morph(h0);
        }
        if (useMilitary) {
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
