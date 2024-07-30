#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <SimpleModbusSlave.h>

Adafruit_ADS1115 ads1;
Adafruit_ADS1115 ads2;
Adafruit_ADS1115 ads3;

float temperature[3][4]; // Declare the temperature array


SimpleModbusSlave slave(1);  // Initialize the Modbus slave with the ID 1
uint16_t regs[12];           // Allocate a mapping of 12 values (3 chips, 4 channels each)
void setup() {
  Serial.begin(9600);
  ads1.begin(0x49);  // Address 0x49
  ads2.begin(0x48);  // Address 0x48
  ads3.begin(0x4B);  // Address 0x4B

  // Set up Modbus communication at 9600 baud
  slave.setup(9600);
}
void loop() {
  // Read temperature from each channel of each chip
  for (int chip = 0; chip < 3; chip++) {
    for (int channel = 0; channel < 4; channel++) {
      int16_t adcValue;
      if (chip == 0) {
        adcValue = ads1.readADC_SingleEnded(channel);
      } else if (chip == 1) {
        adcValue = ads2.readADC_SingleEnded(channel);
      } else if (chip == 2) {
        adcValue = ads3.readADC_SingleEnded(channel);
      }

      // Convert ADC value to voltage
      float voltage = adcValue * 0.1875 / 1000.0;

      // Convert voltage to temperature using LM35's characteristics
      temperature[chip][channel] = voltage * 10.0; // LM35 has 10 mV per degree Celsius sensitivity

      // Update the corresponding register with the temperature value (in tenths of a degree)
      regs[chip * 4 + channel] = int(temperature[chip][channel] * 10);
    }
  }

  // Handle Modbus requests
  slave.loop(regs, sizeof(regs) / sizeof(regs[0]));
}
