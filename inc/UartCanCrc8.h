#ifndef _UARTCANCRC8_H_
#define _UARTCANCRC8_H_

#define CRC8_TABLE_SIZE			256
#define POLY        			0x4d
namespace uartcan{

class UartCanCrc8
{
public:
	UartCanCrc8();
	virtual ~UartCanCrc8();

	
	unsigned char  crc8creator(unsigned char *msg,unsigned char start,unsigned char len);
	int    		   detectcrc8(unsigned char *msg,unsigned char start,unsigned char len);
	
private:
	unsigned char  crc_table[CRC8_TABLE_SIZE];
	void     	   crc8_populate_lsb(unsigned char table[CRC8_TABLE_SIZE], unsigned char polynomial);
	unsigned char  crc8( unsigned char *pdata, unsigned char nbytes, unsigned char crc);
};

};
#endif
