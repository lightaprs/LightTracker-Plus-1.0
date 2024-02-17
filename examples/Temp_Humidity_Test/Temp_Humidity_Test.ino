#include "SparkFun_SHTC3.h" //https://github.com/sparkfun/SparkFun_SHTC3_Arduino_Library

#define SDA_PIN 3
#define SCL_PIN 4

SHTC3 mySHTC3;
bool metric = true;

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN,SCL_PIN);
  delay(7000);
  Serial.println(F("Temperature and Humidity Sensor (SHTC3) Test Started..."));
  setup_SHTC3Sensor();
}

void setup_SHTC3Sensor()
{
    Serial.println(F("SHTC3 Temp & Humidity Sensor init..."));

    SHTC3_Status_TypeDef result = mySHTC3.begin();

    if(result==SHTC3_Status_Nominal){
        Serial.println(F("SHTC3 Temp & Humidity Sensor init done!"));
    } else {
        Serial.print(F("SHTC3 Temp & Humidity Sensor init failed: ")); Serial.println(result);
    }

}

void loop() {
 SHTC3_Status_TypeDef result = mySHTC3.update(); 
    //SHTC3 sensor is not calibrated and readings are a little bit higher due to interal board heat.
    if(result==SHTC3_Status_Nominal){
      if(metric) {
          Serial.println("Temp: "+ String(mySHTC3.toDegC()) + "C Humiditiy(%): "+ String(mySHTC3.toPercent()));
      }else {
          Serial.println("Temp: "+ String(mySHTC3.toDegF()) + "C Humiditiy(%): "+ String(mySHTC3.toPercent()));
        }
     
    } else {
        Serial.print(F("SHTC3 temperature reading failed: ")); Serial.println(result);
    }      


 delay(1000);
}
