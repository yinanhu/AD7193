

/* blah blah */

#include <SPI.h>
#include <AD7193.h>


AD7193 AD7193;

void setup() {
  
  ///////////////////////////
  // setup Serial and SPI
  ///////////////////////////
  
  Serial.begin(9600);
  delay(1000);
  
  AD7193.begin();

 
  // register check at initial reset
  //AD7193.ReadRegisterMap();

  ////////////////////////////////////////////
  // add function for setting up thermocouple?
  // Would need a thermocouple_tempToVoltage function to determine min/max voltage to measure
  // Would need a PGS Setup function that could calculate Gain factor, given a min/max voltage range value
  ////////////////////////////////////////////


  ///////////////////////////////////
  // Device setup
  ///////////////////////////////////
  
  AD7193.AppendStatusValuetoData();  //This might be a requirement - functions will be simpler if I can assume this is true  
  
  AD7193.SetPGAGain(8);

  AD7193.SetAveraging(100);
  

  /////////////////////////////////////
  // Calibrate with given PGA settings - need to recalibrate if PGA setting is changed
  /////////////////////////////////////
  
  AD7193.Calibrate();

  // Check register map values
  AD7193.ReadRegisterMap();
  

  //////////////////////////////////////
  
  Serial.println("\nBegin AD7193 conversion - single conversion (pg 35 of datasheet, figure 25)");
}


void loop() {

  unsigned long channel0;
  unsigned long channel1;
  unsigned long channel8;
  
  channel0 = AD7193.ReadADCChannel(0);
  channel1 = AD7193.ReadADCChannel(1);
  channel8 = AD7193.ReadADCChannel(8);

  Serial.print("\nRaw Data out - CH0: ");
  Serial.print(channel0 >> 8, HEX);

  Serial.print("\tCH1: ");
  Serial.print(channel1 >> 8, HEX);

  Serial.print("\tCH8 (temperature): ");
  Serial.println(channel8 >> 8, HEX);

  // report channel 0 voltage
  Serial.print("\n\t\tChannel 0 Voltage: ");
  Serial.print(AD7193.BinaryToVoltage(channel0 >> 8), 5);
  

  // Convert AD7193 data to temperature for IC Termerature channel (8)
  float ambientTemp = AD7193.BinaryToTemperatureDegC(channel8 >> 8);
  float referenceVoltage = Thermocouple_Ktype_TempToVoltageDegC(ambientTemp);

  // measure thermocouple voltage, and compensate for CJC
  float thermocoupleVoltage = AD7193.BinaryToVoltage(channel1 >> 8);
  float compensatedVoltage = thermocoupleVoltage + referenceVoltage;
  float compensatedTemperature = Thermocouple_Ktype_VoltageToTempDegC(compensatedVoltage);

  Serial.print("\n\t\tChannel 1 Compensated Thermocouple Voltage Measurement: ");
  Serial.print(compensatedTemperature, 3);  Serial.println(" degC");
  Serial.println("\t\t\tThermocouple Measurement Details:");
  Serial.print("\t\t\tThermocouple Voltage: ");  Serial.println(thermocoupleVoltage, 5);
  Serial.print("\t\t\tReference Temp: ");  Serial.println(ambientTemp, 5);
  Serial.print("\t\t\tReference Voltage: ");  Serial.println(referenceVoltage, 5);
  
  delay(100);
}









