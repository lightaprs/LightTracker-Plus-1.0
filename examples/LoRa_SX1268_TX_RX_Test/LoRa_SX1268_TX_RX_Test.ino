#include <RadioLib.h> //https://github.com/jgromes/RadioLib
#include "SPI.h"

// uncomment the following only on one
// of the nodes to initiate the pings
//#define INITIATING_NODE

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

// this function is called when a complete packet
// is transmitted or received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
void setFlag(void) {
  // we sent or received a packet, set the flag
  operationDone = true;
}

int state;

void setup() {
  Serial.begin(115200);
  delay(7000);
  Serial.println(F("LoRa SX1268 TX RX (Ping Pong) Test Started..."));
  setupLoRa();
  
  // set the function that will be called
  // when new packet is received
  radio.setDio1Action(setFlag);

    #if defined(INITIATING_NODE)
    // send the first packet on this node
    Serial.println(F("[SX1268] Sending first packet ... "));
    transmissionState = radio.startTransmit("Light Tracker Plus PingPong First Message");
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


void loop() {
  // check if the previous operation finished
  if(operationDone) {
    // reset flag
    operationDone = false;

    if(transmitFlag) {
      // the previous operation was transmission, listen for response
      // print the result
      if (transmissionState == RADIOLIB_ERR_NONE) {
        // packet was successfully sent
        Serial.println(F("transmission finished!"));

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

      // wait 5 seconds before transmitting again
      delay(5000);
      digitalWrite(LORA_TX_PIN,HIGH); //TX ENABLE
      digitalWrite(LORA_RX_PIN,LOW);  //RX DISABLE
      delay(100);
      // send another one
      Serial.print(F("[SX1268] Sending another packet ... "));
      transmissionState = radio.startTransmit("Light Tracker Plus PingPong Loop Message");
      transmitFlag = true;
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
    Serial.println(F("LoRa Radio Module Initializing ... "));
    state = radio.begin();
    if (state == RADIOLIB_ERR_NONE) {
        Serial.println(F("LoRa Radio Module Initializing success... "));
    } else {
        Serial.println(F("LoRa Radio Module Initializing failed... "));    
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

}
