// gbcartslurp.c
//

#include <stdio.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <mcp23017.h>
#include "mcp23x0817.h"
//#include <mcp23008.h>

//#define _TEST

//#define  AddrBusBase 100
#define	 DataBusBase 120
#define  AncilliaryBase 128
const int wrPin = 0;
const int rdPin = 1;
const int mreqPin = 2;
const int allHigh = 0x07;
const int readPins = 0x01;
const int writePins = 0x06;
int addrFD=0;
int dataFD=0;
char gameTitle[17];
int cartridgeType = 0;
int romSize = 0;
int ramSize = 0;
int romBanks = 2; // Default 32K
int ramBanks = 1;
//only going to touch bank A on the data bus chip
void changeDataBusMode(int mode)
{
	int data;
	if(mode == OUTPUT)
		data = 0x00;
	else
		data =0xFF;
	wiringPiI2CWriteReg8(dataFD, MCP23x17_IODIRA, data);
}
void writeMCPByte(int fd, int data, int AB)
{
	int reg=0;
	if(AB == 0)
		reg=MCP23x17_GPIOA;
	else
		reg=MCP23x17_GPIOB;
	wiringPiI2CWriteReg8(fd, reg, data);
	
}
unsigned char readMCPByte(int fd, int AB)
{
	int reg=0;
	if(AB == 0)
		reg=MCP23x17_GPIOA;
	else
		reg=MCP23x17_GPIOB;
	return (unsigned char)wiringPiI2CReadReg8(fd, reg);
	
}
void pinWrite(int pin, int mode)
{
	int bit,old;
	old = wiringPiI2CReadReg8(dataFD, MCP23x17_OLATB);
	bit = 1 << (pin&7);
	if (mode == LOW)
		old &= (~bit);
	else
		old |= bit;
	
	wiringPiI2CWriteReg8(dataFD, MCP23x17_GPIOB, old);
}
unsigned char readGBByte(unsigned short addr)
{
	unsigned char dataByte = 0xfe;
	//put the address onto the bus
	//split the addr into lower and upper bytes
	int lower=0;
	int upper=0;
	lower = addr & 0x00ff;
	upper = addr >> 8;
	writeMCPByte(addrFD, lower, 0);
	writeMCPByte(addrFD, upper, 1);

	//set read mode
	writeMCPByte(dataFD,readPins,1);
	//pinWrite(wrPin,HIGH);//0
	//pinWrite(mreqPin,LOW);//2
	//pinWrite(rdPin,LOW);//1
#ifdef _TEST
	delay(1000);
#endif	
	delayMicroseconds(2);

	dataByte = (unsigned char)readMCPByte(dataFD,0);

	//set nothing mode
	//pinWrite(mreqPin,HIGH);//2
	//pinWrite(rdPin,HIGH);//1
	writeMCPByte(dataFD,allHigh,1);
	return dataByte;
}
void writeGBByte(unsigned short addr, unsigned char data)
{
	
	//put the address onto the bus
	//split the addr into lower and upper bytes
	int lower=0;
	int upper=0;
	lower = addr & 0x00ff;
	upper = addr >> 8;
	writeMCPByte(addrFD, lower, 0);
	writeMCPByte(addrFD, upper, 1);


	//pinWrite(wrPin,HIGH);
	
	changeDataBusMode(OUTPUT);
	writeMCPByte(dataFD,(int)data,0);
	
	//const int wrPin = 0;
//const int rdPin = 1;
//const int mreqPin = 2;
	writeMCPByte(dataFD,writePins,1);
	//pinWrite(mreqPin,LOW);
	//pinWrite(wrPin,LOW);
#ifdef _TEST
	delay(3000);
#endif
	delay(2);
	writeMCPByte(dataFD,allHigh,1);
	//pinWrite(mreqPin,HIGH);
	//pinWrite(wrPin,HIGH);

	changeDataBusMode(INPUT);
	//return dataByte;
}
int mcpSetup(int i2cAddress)
{
	int fd;
	if((fd = wiringPiI2CSetup (i2cAddress)) >0)
		wiringPiI2CWriteReg8 (fd, MCP23x17_IOCON, IOCON_INIT);
	printf("addr: %X, fd: %d\r\n",i2cAddress,fd);
	return fd;
	
}
void setIO()
{
	//set as outputs
	wiringPiI2CWriteReg8 (addrFD, MCP23x17_IODIRA, 0x00);
	wiringPiI2CWriteReg8 (addrFD, MCP23x17_IODIRB, 0x00);
	//data bus chip
	wiringPiI2CWriteReg8 (dataFD, MCP23x17_IODIRA, 0xFF);
	wiringPiI2CWriteReg8 (dataFD, MCP23x17_IODIRB, 0x00);
}

void test()
{
	//writeMCPByte(addrFD,0xff,0);
	//writeMCPByte(addrFD,0xff,1);
	//delay(1000);
	//writeMCPByte(addrFD,0x00,0);
	//delay(500);
	for(int i=0;i<8;++i)
	{
	//	int data = 1 << i;
		//readGBByte(0xff);
		//writeMCPByte(dataFD,0xff,0);
	//	pinWrite(i,HIGH);
		delay(1000);
	//	pinWrite(i,LOW);
		writeGBByte(0x2100, 0xff-i);
	//	
	}
	//0x
}
void readCartInfo()
{
	char outString[255];
	unsigned int addr = 0x0134;
	for (; addr <= 0x0143; addr++) {
		gameTitle[(addr - 0x0134)] = (char)readGBByte(addr);
	}
	gameTitle[16] = '\0';
	sprintf(outString, "game: %s",gameTitle);
	puts(outString);
	

	
	cartridgeType = readGBByte(0x0147);
	romSize = readGBByte(0x0148);
	ramSize = readGBByte(0x0149);
	romBanks = 2; // Default 32K
	switch(romSize)
	{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		{
			int i;
			for (i=0;i<romSize;++i)
				romBanks *= 2;
				
			if(cartridgeType > 0 && cartridgeType <=3)
			{
				if (romSize == 5)
					romBanks--;
				if (romSize == 6)
					romBanks -= 3;
			}
		}
		break;
		case 82:
			romBanks = 72;
			break;
		case 83:
			romBanks = 80;
			break;
		case 84:
			romBanks = 96;
	}

	ramBanks = 1; // Default 8K RAM
	if (ramSize == 3) { ramBanks = 4; }
	if (ramSize == 4){ ramBanks = 16; } // GB Camera
}
int DumpRom()
{
	char gameFile[25];
	sprintf(gameFile, "%s.gb", gameTitle);
	puts(gameFile);
	FILE* fp = fopen(gameFile, "w+"); //create a new file to write into. overwrite an existing
	if (!fp) 
	{
		perror("File opening failed");
		return 1;
	}
	//FILE* logfp = fopen("logfile.bin","w+");
	//if(!logfp)
	//{
	//	perror("log opening failed");
	//	return 1;
	//}
	//fwrite(gameTitle,25,1,logfp);
	//fclose(logfp);
	//fclose(fp);
	//return 0;

	int addr = 0;
	//char infoString[100];

	printf("**********DUMPING ROM %s ************\r\n",gameFile);
	//dump ROM
	//puts(infoString);
	unsigned char readData[64];
	// Read x number of banks 
	int byteCount = romBanks  * 0x4000;
	printf("%d bytes to read\r\n",byteCount);
	char buf[6];
	int y;
	for (y = 1; y < romBanks; y++) 
	{
		printf("------Dumping bank %d of %d\r\n\r\n[", y, romBanks -1);
		//puts(infoString);

		writeGBByte(0x2100, y); // Set ROM bank 
		if (y > 1) 
			addr = 0x4000; 
		//int j;
		for (int j=1; addr <= 0x7FFF; addr += 64) 
		{
			memset(readData, 0, 64);
			int i;
			for (i = 0; i < 64; i++)
			{
				unsigned char ch = readGBByte((unsigned short)addr + i);
				readData[i] = ch;
				unsigned short tmp = (unsigned short)addr+i;
				sprintf(buf,"%05d",tmp);
				//fwrite(buf,5,1,logfp);
				//fwrite(": ",2,1,logfp);
				sprintf(buf,"%02X\n\r",ch);
				//fwrite(buf,4,1,logfp);
			}
			fwrite(readData, 1, 64,fp);
			printf("#");
			fflush(stdout);
			if (j++ % 32 == 0)
				puts(".");
	
		}
		puts("]");
	
	}
	//fclose(logfp);
	fclose(fp);
	return 0;
}
int WriteRam(char* FileName)
{
	// MBC2 Fix (unknown why this fixes it, maybe has to read ROM before RAM?)
	readGBByte(0x0134);
	unsigned char writeData[64];
	int addr = 0;
	unsigned int endaddr = 0;
	if (cartridgeType == 6 && ramSize == 0) 
		endaddr = 0xA1FF; // MBC2 512bytes (nibbles)
	if (ramSize == 1) 
		endaddr = 0xA7FF; // 2K RAM
	if (ramSize > 1) 
		endaddr = 0xBFFF;  // 8K RAM	
	// Does cartridge have RAM
	if (endaddr > 0) 
	{
		//char saveFile[25];
		//sprintf(saveFile, "%s.sav", gameTitle);
		//puts(saveFile);
		FILE* fp = fopen(FileName, "r"); //create a new file to write into. overwrite an existing
		if (!fp) 
		{
			perror("File opening failed");
			return 1;
		}		
		printf("**********Writing RAM %s ************\r\n",FileName);
		
		// Initialise MBC
		writeGBByte(0x0000, 0x0A);
		char infoString[100];
		
		// Switch RAM banks
		int bank;
		for (bank = 0; bank < ramBanks; bank++) 
		{
			sprintf(infoString, "------Writing bank %d of %d\r\n\r\n[", bank+1, ramBanks -1);
			puts(infoString);

			writeGBByte(0x4000, bank);

			// Read RAM
			int j = 1;
			for (addr = 0xA000; addr <= endaddr; addr = addr + 64) 
			{
				int i;
				fread(writeData,1,64,fp);
				for (i = 0; i < 64; i++){
					writeGBByte(addr+i, writeData[i]);
					//readData[i] = readGBByte(addr + i);
				}
				//fwrite(readData, 1, 64, fp);
				printf("#");
				fflush(stdout);
				if (j++ % 32 == 0)
					puts(".");
			}
			puts("]");
		}
		fclose(fp);
		// Disable RAM
		writeGBByte(0x0000, 0x00);
	}
	else
		puts("*********No RAM found************");
	return 0;
}
int DumpRam()
{
	// MBC2 Fix (unknown why this fixes it, maybe has to read ROM before RAM?)
	readGBByte(0x0134);
	unsigned char readData[64];
	int addr = 0;
	unsigned int endaddr = 0;
	if (cartridgeType == 6 && ramSize == 0) 
		endaddr = 0xA1FF; // MBC2 512bytes (nibbles)
	if (ramSize == 1) 
		endaddr = 0xA7FF; // 2K RAM
	if (ramSize > 1) 
		endaddr = 0xBFFF;  // 8K RAM	
	// Does cartridge have RAM
	if (endaddr > 0) 
	{
		char saveFile[25];
		sprintf(saveFile, "%s.sav", gameTitle);
		puts(saveFile);
		FILE* fp = fopen(saveFile, "w+"); //create a new file to write into. overwrite an existing
		if (!fp) 
		{
			perror("File opening failed");
			return 1;
		}		
		printf("**********DUMPING RAM %s ************\r\n",saveFile);
		
		// Initialise MBC
		writeGBByte(0x0000, 0x0A);
		char infoString[100];
		
		// Switch RAM banks
		int bank;
		for (bank = 0; bank < ramBanks; bank++) 
		{
			sprintf(infoString, "------Dumping bank %d of %d\r\n\r\n[", bank+1, ramBanks -1);
			puts(infoString);

			writeGBByte(0x4000, bank);

			// Read RAM
			int j = 1;
			for (addr = 0xA000; addr <= endaddr; addr = addr + 64) 
			{
				int i;
				for (i = 0; i < 64; i++){
					readData[i] = readGBByte(addr + i);
				}
				fwrite(readData, 1, 64, fp);
				printf("#");
				fflush(stdout);
				if (j++ % 32 == 0)
					puts(".");
			}
			puts("]");
		}
		fclose(fp);
		// Disable RAM
		writeGBByte(0x0000, 0x00);
	}
	else
		puts("*********No RAM found************");
	return 0;
	
}
int main(int argc, char* argv[])
{
	//init wiringpi
	wiringPiSetup();
	mcp23017Setup(100,0x20);
	addrFD = mcpSetup(0x20);
	dataFD = mcpSetup(0x21);
	if(addrFD < 0 || dataFD < 0)
	{
		perror("I2C setup failed");
		puts("Setup Failed\r\n");
		return 1;
	}
	
	//mcp23017Setup(AddrBusBase,0x20);
	//mcp23017Setup(DataBusBase,0x21);
	setIO();
	writeMCPByte(dataFD,allHigh,1);
	//pinWrite(rdPin, HIGH);
	//pinWrite(wrPin, HIGH);
	//pinWrite(mreqPin, HIGH);
	
#ifdef _TEST
	
		test();
		return 0;
#endif
	
	// Read Cartridge Header
	readCartInfo();
	
	if(DumpRom())
		return 1;
	
	//dump ram if there is one
	if(DumpRam())
		return 1;


	puts("Done");

	return 0;
}
