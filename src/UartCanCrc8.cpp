#include "UartCanCrc8.h"
#include <stdio.h>
namespace uartcan{
UartCanCrc8::UartCanCrc8()
{
	printf("UartCanCrc8()\n" );
	crc8_populate_lsb(crc_table ,POLY);
}
UartCanCrc8::~UartCanCrc8()
{
	printf("~UartCanCrc8()\n" );
}
void UartCanCrc8 ::crc8_populate_lsb(unsigned char table[CRC8_TABLE_SIZE], unsigned char polynomial)
{
	int i, j;
	unsigned char t = 1;
	table[0] = 0;

	for (i = (CRC8_TABLE_SIZE >> 1); i; i >>= 1) {
		t = (t >> 1) ^ (t & 1 ? polynomial : 0);
		for (j = 0; j < CRC8_TABLE_SIZE; j += 2*i)
			table[i+j] = table[j] ^ t;
	}
}

unsigned char UartCanCrc8 :: crc8( unsigned char *pdata, unsigned char nbytes, unsigned char crc)
{
	while (nbytes-- > 0){
		crc = crc_table[(crc ^ *pdata++) & 0xff];
	}
	
	return crc;
}
unsigned char UartCanCrc8 :: crc8creator(unsigned char *msg,unsigned char start,unsigned char len)
{
	unsigned char crc=0;
	crc=crc8(&msg[start],len,0);
	return crc;
}

int UartCanCrc8::detectcrc8(unsigned char *msg,unsigned char start,unsigned char len)
{
	unsigned char crc=crc8creator(msg,start,len-1);
	if(crc==msg[len-1]){
		return 1;
	}
	fprintf(stderr,"crc not right len %d %d %d\r\n",len ,crc,msg[len-1]);
	return 0;
}

};