//https://projecthub.arduino.cc/Ninety99/small-covid-medi-kit-spo2-heart-rate-ecg-with-matlab-2c56b8
#include  <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include "MAX30100.h"

#define REPORTING_PERIOD_MS     1000
#define SAMPLING_RATE           MAX30100_SAMPRATE_100HZ

#define IR_LED_CURRENT          MAX30100_LED_CURR_50MA
#define RED_LED_CURRENT         MAX30100_LED_CURR_7_6MA

#define PULSE_WIDTH             MAX30100_SPC_PW_1600US_16BITS
#define HIGHRES_MODE            true

MAX30100 sensor;
PulseOximeter pox;

uint32_t tsLastReport = 0;

void  onBeatDetected()
{               
   //Serial.println();
   Serial.print(",");
   Serial.println(pox.getSpO2());
}

void  setup()
{
    Serial.begin(115200);

    pinMode(10, INPUT); //for L0+
    pinMode(11, INPUT); //for L0-
    
    sensor.begin(); //Get raw values
    sensor.setMode(MAX30100_MODE_SPO2_HR);
    sensor.setLedsCurrent(IR_LED_CURRENT, RED_LED_CURRENT);
    sensor.setLedsPulseWidth(PULSE_WIDTH);
    sensor.setSamplingRate(SAMPLING_RATE);
    sensor.setHighresModeEnabled(HIGHRES_MODE);
    
    pox.begin();
 // pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
    
}

void loop()
{
    Serial.print(analogRead(A0));
    Serial.println(", ,");
    uint16_t  irValue, red;

    sensor.update();
    pox.update();
    
    sensor.getRawValues(&irValue,  &red);
    if (irValue < 7000){       //If no finger is detected it inform the  user and put the average BPM to 0 or it will be stored for the next measure              
     //Serial.println("Please Place your finger "); 

    }
        
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {

        if(irValue  > 7000){ //If a finger is detected                                      
        //Serial.println();
        Serial.print(",");
        Serial.println(pox.getSpO2());
        
        pox.setOnBeatDetectedCallback(onBeatDetected);                        //If  a heart beat is detected
        
        //Serial.println(); 
        Serial.print(",");
        Serial.println(pox.getSpO2());
        tsLastReport  = millis();
      }//inner if for finger detect
  }//outer if for Report
}//  loop end
