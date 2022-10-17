# ModBus
Using the Arduino ModBus and RS485 libraries to retrieve data from a Finder energy meter using a Arduino MKR Wifi 1010

## Request Form

| Device Adress | Function Code | Starting Register |  Register Count | CRC Code |  
| ----------- | ----------- |----------- |----------- |----------- |
| 0x21      | INPUT REGISTERS       | 0x0011       | 2       |

### Example 
```
(!ModbusRTUClient.requestFrom(0x21, INPUT_REGISTERS, 30017, 2))
```