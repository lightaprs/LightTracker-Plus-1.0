/*
  RadioLib LoRaWAN End Device ABP Example

  This example sets up a LoRaWAN node using ABP (activation
  by personalization). Before you start, you will have to
  register your device at https://www.thethingsnetwork.org/
  After your device is registered, you can run this example.
  The device will start uploading data directly,
  without having to join the network.

  LoRaWAN v1.1 requires the use of EEPROM (persistent storage).
  Please refer to the 'persistent' example once you are familiar
  with LoRaWAN.
  Running this examples REQUIRES you to check "Resets DevNonces"
  on your LoRaWAN dashboard. Refer to the network's 
  documentation on how to do this.
  
  For default module settings, see the wiki page
  https://github.com/jgromes/RadioLib/wiki/Default-configuration

  For full API reference, see the GitHub Pages
  https://jgromes.github.io/RadioLib/
*/

// include the library
#include <RadioLib.h>

// SX1278 has the following connections:
// NSS pin:   10
// DIO0 pin:  2
// RESET pin: 9
// DIO1 pin:  3
SX1278 radio = new Module(10, 2, 9, 3);

// create the node instance on the EU-868 band
// using the radio module and the encryption key
// make sure you are using the correct band
// based on your geographical location!
LoRaWANNode node(&radio, &EU868);

void setup() {
  Serial.begin(9600);

  // initialize SX1278 with default settings
  Serial.print(F("[SX1278] Initializing ... "));
  int state = radio.begin();
  if(state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while(true);
  }

  // device address - this number can be anything
  // when adding new end device in TTN, you can generate this number,
  // or you can set any value you want, provided it is unique
  uint32_t devAddr = 0x12345678;

  // select some encryption keys which will be used to secure the communication
  // there are two of them - network key and application key
  // because LoRaWAN uses AES-128, the key MUST be 16 bytes (or characters) long

  // network key is the ASCII string "topSecretKey1234"
  uint8_t nwkSKey[] = { 0x74, 0x6F, 0x70, 0x53, 0x65, 0x63, 0x72, 0x65,
                        0x74, 0x4B, 0x65, 0x79, 0x31, 0x32, 0x33, 0x34 };

  // application key is the ASCII string "aDifferentKeyABC"
  uint8_t appSKey[] = { 0x61, 0x44, 0x69, 0x66, 0x66, 0x65, 0x72, 0x65,
                        0x6E, 0x74, 0x4B, 0x65, 0x79, 0x41, 0x42, 0x43 };

  // prior to LoRaWAN 1.1.0, only a single "nwkKey" is used
  // when connecting to LoRaWAN 1.0 network, "appKey" will be disregarded
  // and can be set to NULL

  // some frequency bands only use a subset of the available channels
  // you can select the specific band or set the first channel and last channel
  // for example, either of the following corresponds to US915 FSB2 in TTN
  /*
    node.selectSubband(2);
    node.selectSubband(8, 15);
  */

  // if using EU868 on ABP in TTN, you need to set the SF for RX2 window manually
	/*
    node.rx2.drMax = 3;
  */

  // to start a LoRaWAN v1.1 session, the user should also provide 
  // fNwkSIntKey and sNwkSIntKey similar to nwkSKey and appSKey
  /*
    state = node.beginABP(devAddr, nwkSKey, appSKey, fNwkSIntKey, sNwkSIntKey);
  */

  // start the device by directly providing the encryption keys and device address
  Serial.print(F("[LoRaWAN] Attempting over-the-air activation ... "));
  state = node.beginABP(devAddr, nwkSKey, appSKey);
  if(state == RADIOLIB_ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while(true);
  }

}

// counter to keep track of transmitted packets
int count = 0;

void loop() {
  // send uplink to port 10
  Serial.print(F("[LoRaWAN] Sending uplink packet ... "));
  String strUp = "Hello World! #" + String(count++);
  String strDown;
  int state = node.sendReceive(strUp, 10, strDown);
  if(state == RADIOLIB_ERR_NONE) {
    Serial.println(F("received a downlink!"));

    // print data of the packet (if there are any)
    Serial.print(F("[LoRaWAN] Data:\t\t"));
    if(strDown.length() > 0) {
      Serial.println(strDown);
    } else {
      Serial.println(F("<MAC commands only>"));
    }

    // print RSSI (Received Signal Strength Indicator)
    Serial.print(F("[LoRaWAN] RSSI:\t\t"));
    Serial.print(radio.getRSSI());
    Serial.println(F(" dBm"));

    // print SNR (Signal-to-Noise Ratio)
    Serial.print(F("[LoRaWAN] SNR:\t\t"));
    Serial.print(radio.getSNR());
    Serial.println(F(" dB"));

    // print frequency error
    Serial.print(F("[LoRaWAN] Frequency error:\t"));
    Serial.print(radio.getFrequencyError());
    Serial.println(F(" Hz"));
  
  } else if(state == RADIOLIB_ERR_RX_TIMEOUT) {
    Serial.println(F("no downlink!"));
  
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
  }

  // wait before sending another packet
  delay(30000);
}
