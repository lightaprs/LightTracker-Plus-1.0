#include "OneButton.h" //https://github.com/mathertel/OneButton/
#include <Adafruit_SSD1306.h>//https://github.com/adafruit/Adafruit_SSD1306
#include <Adafruit_GFX.h>//https://github.com/adafruit/Adafruit-GFX-Library and https://github.com/adafruit/Adafruit_BusIO
#include "SparkFun_SHTC3.h" //https://github.com/sparkfun/SparkFun_SHTC3_Arduino_Library
#include <TinyGPSPlus.h> //https://github.com/mikalhart/TinyGPSPlus
#include <RadioLib.h> //https://github.com/jgromes/RadioLib
#include "SPI.h"
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>

#define BUTTON_PIN 0
#define LED_PIN 16
#define VOLTAGE_READ_PIN 1
#define ADC_MULTIPLIER 5.65
#define ADC_REFERENCE REF_3V3

#define SDA_PIN 3
#define SCL_PIN 4
#define OLED_RST_PIN -1
#define SCREEN_WIDTH    128     // OLED display width, in pixels
#define SCREEN_HEIGHT   64      // OLED display height, in pixels
#define SCREEN_ADDRESS  0x3C    ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

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

uint16_t lastFetchedYear = 2100;
uint32_t lastFetchedSats = -1;
bool lastLocValid = false;

// uncomment the following only on one
// of the nodes to initiate the pings
#define INITIATING_NODE

#define SX126X_MAX_POWER 22 //22 is the max value for SX126X modules but E22-400M30S amplifies this to 30dBm. So do not change this.

#define LORA_VCC_PIN 21
#define LORA_RX_PIN 42
#define LORA_TX_PIN 14
#define LORA_SCK_PIN 12
#define LORA_MISO_PIN 13
#define LORA_MOSI_PIN 11
#define LORA_CS_PIN 10
#define LORA_RST_PIN 9
#define LORA_IRQ_PIN 5
#define LORA_BUSY_PIN 6

//******OneButton************/
OneButton btn;
static bool buttonPressed = false;

//*******Display************/
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST_PIN);

//****SHTC3*****/
SHTC3 mySHTC3;

//******LoRa************/
SPIClass loraSPI(FSPI);
SX1268 radio = new Module(LORA_CS_PIN, LORA_IRQ_PIN, LORA_RST_PIN, LORA_BUSY_PIN,loraSPI);  
// save transmission state between loops
int transmissionState = RADIOLIB_ERR_NONE;
// flag to indicate transmission or reception state
bool transmitFlag = false;
// flag to indicate that a packet was sent or received
volatile bool operationDone = false;
#if defined(ESP8266) || defined(ESP32)
  ICACHE_RAM_ATTR
#endif

void IRAM_ATTR checkTicks() {
  // keep watching the push button:
  btn.tick();
}

void setFlag(void) {
  // we sent or received a packet, set the flag
  operationDone = true;
}
int state;

//*********WI-FI Settings**********/
const char* ssid = "CHANGE_ME";
const char* password = "CHANGE_ME";


void setup() {
  Serial.begin(115200);
  delay(7000);
  Serial.println(F("Light Tracker Plus Full Test Started..."));
  
  pinMode(LED_PIN, OUTPUT);
  ledBlink(true);
  Serial.println(F("LED Blinked...[DONE]"));
  
  Wire.begin(SDA_PIN,SCL_PIN);
  Serial.println(F("Wire.begin()...[DONE]"));
  
  setup_Button();
  setupDisplay();
  setupGPS();
  setup_SHTC3Sensor();
  Serial.print(F("Voltage: "));Serial.println(readBatteryVoltage());
  setupLoRa();
  setupWiFi();


}

void loop() {
   btn.tick();
   updateGPSData();
   if(gps.satellites.value() != lastFetchedSats || gps.location.isValid() != lastLocValid || gps.date.year() < lastFetchedYear){
      lastFetchedSats = gps.satellites.value();
      lastFetchedYear = gps.date.year();
      lastLocValid = gps.location.isValid();
      printGPSData();
      printTempHumidity();
      Serial.print(F("Voltage: "));Serial.println(readBatteryVoltage());
    }
   if (!gps.location.isValid()){
      gpsSearchLedBlink();
    }

    loopLoRa();
    
    delay(100);
}

void setup_SHTC3Sensor()
{
    Serial.print(F("SHTC3 Temp & Humidity Sensor init..."));

    SHTC3_Status_TypeDef result = mySHTC3.begin();

    if(result==SHTC3_Status_Nominal){
        Serial.println(F("[DONE]"));
    } else {
        Serial.println(F("[DONE]"));
    }
    printTempHumidity();

}

void printTempHumidity(){
    SHTC3_Status_TypeDef result = mySHTC3.update(); 
    //SHTC3 sensor is not calibrated and readings are a little bit higher due to interal board heat.
    if(result==SHTC3_Status_Nominal){
          Serial.println("Temp: "+ String(mySHTC3.toDegC()) + "C Humiditiy(%): "+ String(mySHTC3.toPercent()));
    } else {
        Serial.print(F("SHTC3 temperature reading failed: ")); Serial.println(result);
    }      
  
  
  }

void setupGPS()
{
  pinMode(GPS_VCC_PIN,OUTPUT); 
  GpsON;
  delay(100);
  serialGPS.begin(GPS_BAUDRATE,SERIAL_8N1,GPS_RX_PIN,GPS_TX_PIN);
  Serial.println(F("GPS Begin...[DONE]"));
  delay(1000);
  updateGPSData();
  Serial.println(F("GPS Data Read...[DONE]"));
  bool modeChange = changeGPSMode(GPS_MODE_NORMAL);
  Serial.print(F("GPS Mode Change..."));
  if(modeChange) {
      Serial.println(F("[DONE]"));
    } else {
      Serial.println(F("[FAILED]"));
      }
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
        //Serial.println(F("Mode change acknowledged by Quectel LXX GPS module..."));
        result = true;
      }      
    }
    
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
        Serial.println(F("GPS Data Read...[FAILED]"));
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


void setupWiFi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("[DONE]");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  
}

void setupDisplay() {

  Serial.print(F("SSD1306 Display init..."));

  if(display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("DONE!");  
    display.ssd1306_command(SSD1306_SETCONTRAST);
    display.ssd1306_command(1);
    display.display();
    delay(500);
    Serial.println(F("[DONE]"));  
  } else {
    Serial.println(F("[FAILED]"));
  }
    
}

void ledBlink(bool enabled) {
  if(enabled) {
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
}

void setup_Button()
{
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  btn = OneButton(
  BUTTON_PIN, // Input pin for the button
  true,       // Button is active LOW
  true       // Enable internal pull-up resistor
  );
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), checkTicks, CHANGE);
  btn.attachClick(singleClick);
  btn.attachDoubleClick(doubleClick);
  Serial.println(F("BOOT Button Setup...[DONE]"));
}

void singleClick() {
    Serial.println(F("Button pressed once"));
}

void doubleClick() {
    Serial.println(F("Button pressed twice"));
}

void loopLoRa(){
  // check if the previous operation finished
  if(operationDone) {
    // reset flag
    operationDone = false;

    if(transmitFlag) {
      // the previous operation was transmission, listen for response
      // print the result
      if (transmissionState == RADIOLIB_ERR_NONE) {
        // packet was successfully sent
        Serial.println(F("[SX1268] LoRa Packet Sent [DONE]"));

      } else {
        Serial.print(F("failed, code "));
        Serial.println(transmissionState);
      }
      digitalWrite(LORA_TX_PIN,LOW); //TX DISABLE
      digitalWrite(LORA_RX_PIN,HIGH); //RX ENABLE
      delay(100);
      // listen for response
      radio.startReceive();
      transmitFlag = false;

    } else {
      // the previous operation was reception
      // print data and send another packet
      String str;
      int state = radio.readData(str);

      if (state == RADIOLIB_ERR_NONE) {
        // packet was successfully received
        Serial.println(F("[SX1268] Received packet!"));

        // print data of the packet
        Serial.print(F("[SX1268] Data:\t\t"));
        Serial.println(str);

        // print RSSI (Received Signal Strength Indicator)
        Serial.print(F("[SX1268] RSSI:\t\t"));
        Serial.print(radio.getRSSI());
        Serial.println(F(" dBm"));

        // print SNR (Signal-to-Noise Ratio)
        Serial.print(F("[SX1268] SNR:\t\t"));
        Serial.print(radio.getSNR());
        Serial.println(F(" dB"));

      }

      /**
      delay(5000);
      digitalWrite(LORA_TX_PIN,HIGH); //TX ENABLE
      digitalWrite(LORA_RX_PIN,LOW);  //RX DISABLE
      delay(100);
      // send another one
      Serial.print(F("[SX1268] Sending another packet ... "));
      transmissionState = radio.startTransmit("Light Tracker Plus Test Loop Message");
      transmitFlag = true;
      */
    }
  
  }  
}

void setupLoRa()
{
    pinMode(LORA_VCC_PIN,OUTPUT);  
    pinMode(LORA_TX_PIN,OUTPUT);
    pinMode(LORA_RX_PIN,OUTPUT);

    digitalWrite(LORA_VCC_PIN,HIGH);
    delay(500);

    loraSPI.begin(LORA_SCK_PIN,LORA_MISO_PIN,LORA_MOSI_PIN,LORA_CS_PIN);

    // initialize LoRa Radio Module with default settings
    Serial.print(F("LoRa Radio Module Initializing..."));
    state = radio.begin();
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("[DONE]"));
    } else {
        Serial.println(F("[FAILED!]"));    
        while (true);
    }

    digitalWrite(LORA_TX_PIN,HIGH); //TX ENABLE
    digitalWrite(LORA_RX_PIN,LOW);  //RX DISABLE

   // set carrier frequency to 433.775 MHz
    if (radio.setFrequency(433.775) == RADIOLIB_ERR_INVALID_FREQUENCY) {
      Serial.println(F("Selected frequency is invalid for this module!"));
      while (true);
    }
      
    // set bandwidth to 125 kHz
    if (radio.setBandwidth(125.0) == RADIOLIB_ERR_INVALID_BANDWIDTH) {
      Serial.println(F("Selected bandwidth is invalid for this module!"));
      while (true);
    }
  
    // set spreading factor to 12
    if (radio.setSpreadingFactor(12) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR) {
      Serial.println(F("Selected spreading factor is invalid for this module!"));
      while (true);
    }
  
    // set coding rate to 5
    if (radio.setCodingRate(5) == RADIOLIB_ERR_INVALID_CODING_RATE) {
      Serial.println(F("Selected coding rate is invalid for this module!"));
      while (true);
    }
  
    // set LoRa sync word
    if (radio.setSyncWord(RADIOLIB_SX126X_SYNC_WORD_PRIVATE) != RADIOLIB_ERR_NONE) {
      Serial.println(F("Unable to set sync word!"));
      while (true);
    }
  
    // set LoRa preamble length to 8 symbols (accepted range is 0 - 65535)
    if (radio.setPreambleLength(8) == RADIOLIB_ERR_INVALID_PREAMBLE_LENGTH) {
      Serial.println(F("Selected preamble length is invalid for this module!"));
      while (true);
    }
  
    // disable CRC
    if (radio.setCRC(false) == RADIOLIB_ERR_INVALID_CRC_CONFIGURATION) {
      Serial.println(F("Selected CRC is invalid for this module!"));
      while (true);
    }
  
     // set output power to 22 dBm (accepted range is -17 - 22 dBm)
    if (radio.setOutputPower(22) == RADIOLIB_ERR_INVALID_OUTPUT_POWER) {
      Serial.println(F("Selected output power is invalid for this module!"));
      while (true);
    }
  
    // set over current protection limit to 140 mA (accepted range is 45 - 140 mA)
    // NOTE: set value to 0 to disable overcurrent protection
    if (radio.setCurrentLimit(140.0) == RADIOLIB_ERR_INVALID_CURRENT_LIMIT) {
      Serial.println(F("Selected current limit is invalid for this module!"));
      while (true);
    }

    // set the function that will be called
    // when new packet is received
    radio.setDio1Action(setFlag);    
  
    #if defined(INITIATING_NODE)
      // send the first packet on this node
      Serial.println(F("[SX1268] Sending first packet ... "));
      transmissionState = radio.startTransmit("Light Tracker Plus Full Test Message");
      transmitFlag = true;
    #else
      // start listening for LoRa packets on this node
      Serial.println(F("[SX1268] Starting to listen ... "));
      state = radio.startReceive();
      if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("success!"));
      } else {
        Serial.print(F("failed, code "));
        Serial.println(state);
        while (true);
      }
    #endif
}

double readBatteryVoltage() {

  u_int8_t num_samples = 10;
  int sum = 0;             
  u_int8_t sample_count = 0;

  while (sample_count < num_samples) {
    sum += analogRead(VOLTAGE_READ_PIN);
    sample_count++;
    delay(10);
  }

  int reading_average = sum / num_samples;

  if(reading_average < 1 || reading_average > 4095) return 0;

  float voltage = ADC_MULTIPLIER * (-0.000000000000016 * pow(reading_average,4) + 0.000000000118171 * pow(reading_average,3)- 0.000000301211691 * pow(reading_average,2)+ 0.001109019271794 * reading_average + 0.034143524634089);

  return voltage;
}
