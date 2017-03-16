//simple I2C bus wrapper from DavidEGrayson/minimu9-ahrs with the following licence:
/*
Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files(the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions :

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef _I2CBus_h
#define _I2CBus_h

#include <stdint.h>
#include "exceptions.h"

class I2CBus
{
public:
    I2CBus(const char * deviceName);
    ~I2CBus();

    void addressSet(uint8_t address);
    void writeByte(uint8_t command, uint8_t data);
    uint8_t readByte(uint8_t command);
    int tryReadByte(uint8_t command);
    void readBlock(uint8_t command, uint8_t size, uint8_t * data);

    int tryReadByte(uint8_t address, uint8_t command)
    {
        addressSet(address);
        return tryReadByte(command);
    }

private:
    int fd;
};

#endif
