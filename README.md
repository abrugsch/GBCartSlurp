# GBCartSlurp
Dumps Gameboy and Gameboy color cartridges with a suitable reader. Code is based on the original @insidegadgets [GBCartRead](https://github.com/insidegadgets/GBCartRead) for arduino but modified to work with the wiringPi library on the Pi. The hardware consists of a Raspberry Pi and 2 MCP23017 I2C 16 way port expanders. configuration to be documented once the design is finalised

### Deps:
[wiringPi](https://projects.drogon.net/raspberry-pi/wiringpi)
specifically the header file 23x017.h for the port name defines

### Improvements:
higher speed may be acheived with the SPI version of the MCP23S17 port expander
