/* -----------------------------------------
 * Finder Energy Meter to Arduino Cloud IoT
 * -----------------------------------------
 * This sketch provides a full bridge between the Finder energy meter and the 
 * Arduino Cloud IoT. This sketch was developed to monitor electricity costs 
 * and usage in Casa Jasmina.
 *
 * Created by Alberto Perro (Officine Innesto)
 * Modified by José Bagur
*/
 
#include <ArduinoRS485.h>
#include <ArduinoModbus.h>

#undef ON
#undef OFF

float voltage;
float current;
float power;
float energy;
float frequency;

unsigned long rate = 10000; // Default refresh rate in ms
unsigned long lastMillis = 0;

void setup() {
  // Initialize serial port at 9600 bauds and wait for it to open
  Serial.begin(9600);
  delay(1500); 

  // Start Modbus RTU client
  if (!ModbusRTUClient.begin(19200)) {
    Serial.println("- Failed to start Modbus RTU Client!");
    while (1);
  }
}

void loop() {
  
  // Update energy meter data and show it via the Serial Monitor
  if (millis() - lastMillis > rate) {
    lastMillis = millis();
  
    voltage = readVoltage();
    delay(100);
    current = readCurrent();
    delay(100);
    power = readPower();
    delay(100);
    frequency = readFreq();
    delay(100);
    energy = readEnergy();
  
    Serial.print("- " + String(voltage, 3) + "V " + String(current, 3) + "A " + String(power, 3) + "W ");
    Serial.println(String(frequency, 3) + "Hz " + String(power, 3) + "kWh");
    delay(100);
  }   
}

/* Functions to read Finder energy meter holding registers
 * For more information: https://gfinder.findernet.com/public/attachments/7E/EN/PRT_Modbus_7E_64_68_78_86EN.pdf
 */

/* 
Function readVoltage()
Description: read voltage value from the Finder energy meter holding registers
*/ 
float readVoltage() {
  float volt = 0.;
  // Send reading request over RS485 
  if (!ModbusRTUClient.requestFrom(0x21, INPUT_REGISTERS, 30017, 2)) {
    // Error handling
    Serial.print("- Failed to read the voltage! ");
    Serial.println(ModbusRTUClient.lastError()); 
  } else {
    // Response handler 
    uint16_t word1 = ModbusRTUClient.read();  // Read word1 from buffer
    uint16_t word2 = ModbusRTUClient.read();  // Read word2 from buffer
    uint32_t millivolt = word1 << 16 | word2; // Join word1 and word2 to retrieve voltage value in millivolts
    volt = millivolt/1000.0;                  // Convert to volts
  }

  return volt;
}

/* 
Function readCurrent()
Description: read current value from the Finder energy meter holding registers
*/
float readCurrent() {        
  float ampere = 0.;
  // Send reading request over RS485      
  if (!ModbusRTUClient.requestFrom(0x21, HOLDING_REGISTERS, 0x0012, 2)) {
    // Error handling   
    Serial.print("- Failed to read the current! ");    
    Serial.println(ModbusRTUClient.lastError());         
  } else {        
    // Response handler 
    uint16_t word1 = ModbusRTUClient.read();  // Read word1 from buffer
    uint16_t word2 = ModbusRTUClient.read();  // Read word2 from buffer
    int32_t milliamp = word1 << 16 | word2;   // Join word1 and word2 to retrieve current value in milliampere
    ampere = milliamp/1000.0;                 // Convert current to ampere
  }

  return ampere;
}

/* 
Function readPower()
Description: read power value from the Finder energy meter holding registers
*/
double readPower() {
  double watt = 0.;
  // Send reading request over RS485
  if (!ModbusRTUClient.requestFrom(0x01, HOLDING_REGISTERS, 0x0025, 3)) {
    // Error handling   
    Serial.print("- Failed to read power! ");
    Serial.println(ModbusRTUClient.lastError());
  } else {
    // Response handler 
    uint16_t word1 = ModbusRTUClient.read();  // Read word1 from buffer
    uint16_t word2 = ModbusRTUClient.read();  // Read word2 from buffer
    uint16_t word3 = ModbusRTUClient.read();  // Read word3 from buffer

    uint64_t milliwatt;

    // Join word1 and word2 to retrieve power value in milliwatt
    if (word1 >> 7 == 0) {
      milliwatt = word1 << 32 | word2 << 16 | word3;
    } else {
      word1 &= 0b01111111;
      milliwatt = 0b1 << 48 | word1 << 32 | word2 << 16 | word3;
    }

    watt = milliwatt/1000.;                   // Convert power to watts
  }

  return watt;
}

/* 
Function readFreq()
Description: read frequency value from the Finder energy meter holding registers
*/
float readFreq() {
  float freq = 0.;
  // Send reading request over RS485
  if (!ModbusRTUClient.requestFrom(0x01, HOLDING_REGISTERS, 0x0040, 2)) {
    // Error handling   
    Serial.print("- Failed to read frequency! ");
    Serial.println(ModbusRTUClient.lastError());
  } else {
    // Response handler 
    uint16_t word1 = ModbusRTUClient.read();  // Read word1 from buffer
    freq = word1/1000.0;                      // Retrieve frequency value
  }
  return freq;
}

/* 
Function readEnergy()
Description: read energy value from the Finder energy meter holding registers
*/
double readEnergy() {
  double kwh = 0.;
  // Send reading request over RS485
  if (!ModbusRTUClient.requestFrom(0x01, HOLDING_REGISTERS, 0x0109, 3)) {
    // Error handling   
    Serial.print("- Failed to read energy! ");
    Serial.println(ModbusRTUClient.lastError());
  } else {
    // Response handler 
    uint16_t word1 = ModbusRTUClient.read();            // Read word1 from buffer
    uint16_t word2 = ModbusRTUClient.read();            // Read word2 from buffer
    uint16_t word3 = ModbusRTUClient.read();            // Read word3 from buffer
    uint64_t dwh = word1 << 32 | word2 << 16 | word3;   // Join word1 and word2 to retrieve energy value in dwh
    kwh = dwh/10000.0;                                  // Convert energy to kwh
  }
  return kwh;
}