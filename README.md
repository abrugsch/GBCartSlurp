# NOTICE: 
~~This project may be deprecated as I can't really get the transfer speed up to where I'd like it for a RPi 1/ZERO. I'm going to experiment with SPI however after playing with a JoeyJoebags the speed difference between using a microcontroller as the interface is phenominal. a cart that takes ~9 minutes to dump using I2C takes mere seconds on a joebags. I know the interface on that is a microcontroller so I'm considering switching totally to a teensy as the interface, unless SPI/23S17 chips can make a significant difference. If the switch to SPI is acceptable, then there are actually minimal changes to the circuit after swapping over to the SPI versions of the chips~~  
The above has changed in the past year. I have got the SPI 23S17's working but they were unreliable. I have found an implementation of SPI reads that does work reliably and will port my code into it when time permits. 


# GBCartSlurp
Dumps Gameboy and Gameboy color cartridges with a suitable reader. Code is based on the original @insidegadgets [GBCartRead](https://github.com/insidegadgets/GBCartRead) for arduino but modified to work with the wiringPi library on the Pi. The hardware consists of a Raspberry Pi and 2 MCP23017 I2C 16 way port expanders.  See GBCartSlurpWiringDiagram.svg for how to connect the Pi and configure the 23017's. Full configuration to be documented once the design is finalised.

### Status:
the code works with the published schematics but is quite slow however it is functional and once I've tried SPI, I'll have an idea what direction this project is going to head in.

here's a vid: (sorry for the long setup time. skips to 3:50 for the meat)

[![Dumping!](http://img.youtube.com/vi/5k2ceMZk8GE/3.jpg)](https://www.youtube.com/watch?v=5k2ceMZk8GE?t=230)

### PCB:
if you want to order a PCB form OSHPark for the I2C variant in the most recent state I had them made then the link is here:

[![Order from OSH Park](https://oshpark.com/assets/badge-5b7ec47045b78aef6eb9d83b3bac6b1920de805e9a0c227658eac6e19a045b9c.png)](https://oshpark.com/shared_projects/dTC1g6Jf)

[![Order from OSH Park](https://644db4de3505c40a0444-327723bce298e3ff5813fb42baeefbaa.ssl.cf1.rackcdn.com/a7b95be807baf7ae4e85514ece911d38.png)](https://oshpark.com/shared_projects/dTC1g6Jf)

You'll need the SSOP variant of MCP23017 to fit the PCB and the 0805 LED's and resistors (anything from 330R to 1K should be fine. just depends on how bright you want them!!) are entirely optional, as are the decoupling capacitors though they are recommended 

### Deps:
[wiringPi](https://projects.drogon.net/raspberry-pi/wiringpi)
specifically the header file 23x017.h for the port name defines

### Improvements:
higher speed may be acheived with the SPI version of the MCP23S17 port expander
