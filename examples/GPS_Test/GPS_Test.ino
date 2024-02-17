#include <TinyGPSPlus.h> //https://github.com/mikalhart/TinyGPSPlus

#define LED_PIN 16
#define GPS_SERIAL_NUM 1
#define GPS_BAUDRATE 9600
#define GPS_RX_PIN 18
#define GPS_TX_PIN 17
#define GPS_VCC_PIN 33
#define GpsON         digitalWrite(GPS_VCC_PIN, LOW)//PNP
#define GpsOFF        digitalWrite(GPS_VCC_PIN, HIGH)

//https://docs.rs-online.com/f450/0900766b8147dbe0.pdf
//https://www.mouser.com/datasheet/2/1052/Quectel_L80-R_GPS_Specification_V1.2-1830014.pdf
enum gps_pmtk_fr_modes {
  GPS_MODE_NORMAL,
  GPS_MODE_FITNESS,
  GPS_MODE_AVIATION,
  GPS_MODE_BALLOON
};

#define GPS_PMTK_FR_MODE_NORMAL_REQ     "$PMTK886,0*28"
#define GPS_PMTK_FR_MODE_FITNESS_REQ    "$PMTK886,1*29"
#define GPS_PMTK_FR_MODE_AVIATION_REQ   "$PMTK886,2*2A"
#define GPS_PMTK_FR_MODE_BALLOON_REQ    "$PMTK886,3*2B"

#define GPS_PMTK_FR_MODE_RESPONSE       "$PMTK001,886,3*36"

TinyGPSPlus gps;
HardwareSerial serialGPS(GPS_SERIAL_NUM);
unsigned long gpsLastProcessedTimeStamp = 0;
bool gps_fix_search_led = true;

void setup() {
  Serial.begin(115200);
  delay(7000);
  Serial.println(F("GPS Test Started..."));
  setupGPS();
  pinMode(LED_PIN, OUTPUT);
}

void setupGPS()
{
  pinMode(GPS_VCC_PIN,OUTPUT); 
  GpsON;
  delay(100);
  serialGPS.begin(GPS_BAUDRATE,SERIAL_8N1,GPS_RX_PIN,GPS_TX_PIN);
  delay(1000);
  bool modeChange = changeGPSMode(GPS_MODE_BALLOON);
  Serial.print(F("Mode Changed?: "));Serial.println(modeChange);
  Serial.println(F(""));
  modeChange = changeGPSMode(GPS_MODE_NORMAL);
  Serial.print(F("Mode Changed?: "));Serial.println(modeChange);
  Serial.println(F(""));
}

void loop() {
 updateGPSData();
 printGPSData();
 if (!gps.location.isValid()){
    gpsSearchLedBlink();
  }
 delay(1000);
}

bool changeGPSMode(u_int8_t cmdType) {
  bool result = false;

    switch (cmdType)
    {
    case GPS_MODE_NORMAL:
      Serial.println(F("GPS mode change request: NORMAL"));
      serialGPS.println(GPS_PMTK_FR_MODE_NORMAL_REQ);  
      break;
    case GPS_MODE_FITNESS:
      Serial.println(F("GPS mode change request: FITNESS"));
      serialGPS.println(GPS_PMTK_FR_MODE_FITNESS_REQ);
      break;
    case GPS_MODE_AVIATION:
      Serial.println(F("GPS mode change request: AVIATION"));
      serialGPS.println(GPS_PMTK_FR_MODE_AVIATION_REQ);    
      break;
    case GPS_MODE_BALLOON:
      Serial.println(F("GPS mode change request: BALLOON"));
      serialGPS.println(GPS_PMTK_FR_MODE_BALLOON_REQ);       
      break;
    default:
      return false;
      break;
    }
    serialGPS.flush();
    delay (250);
    String str="";
    char c;
    while (serialGPS.available() && !result) {
      c=serialGPS.read();
      str.concat(c);
      if ((str.indexOf(GPS_PMTK_FR_MODE_RESPONSE))!=-1) {
        Serial.println(F("Mode change acknowledged by Quectel LXX GPS module..."));
        result = true;
      }      
    }
    Serial.println(str);
    Serial.println(F(""));    
    
  return result;
}

void updateGPSData()
{
    while (serialGPS.available() > 0)
    {
        gps.encode(serialGPS.read());
    }

    if ((millis() - gpsLastProcessedTimeStamp) > 10000 && gps.charsProcessed() < 10)
    {
        Serial.print("No GPS detected: check wiring.");
        while (true);
    }
    gpsLastProcessedTimeStamp = millis();

}

void printGPSData()
{

  Serial.print(F("Sats: ")); 
  if (gps.satellites.isValid())
  {
    Serial.print(gps.satellites.value());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" Altitude: ")); 
  if (gps.altitude.isValid())
  {
    Serial.print(gps.altitude.meters());Serial.print(F("m"));
  }
  else
  {
    Serial.print(F("INVALID"));
  }    

  Serial.print(F(" Speed: ")); 
  if (gps.speed.isValid())
  {
    Serial.print((int)gps.speed.mph());
  }
  else
  {
    Serial.print(F("INVALID"));
  }  

  Serial.print(F(" Course: ")); 
  if (gps.course.isValid())
  {
    Serial.print((int)gps.course.deg());
  }
  else
  {
    Serial.print(F("INVALID"));
  }  

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}

void gpsSearchLedBlink(){
    if(gps_fix_search_led) {
      ledBlink(true);
      gps_fix_search_led = false;
    } else {
      ledBlink(false);
      gps_fix_search_led = true;
    }    
}

void ledBlink(bool enabled) {
  if(enabled) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}
