#include "nrf.h"

extern SPI_HandleTypeDef hspi1;
#define TX_ADR_WIDTH    5   // 5 bytes TX(RX) address width
#define TX_PLOAD_WIDTH  20  // 16 bytes TX payload
unsigned char TX_ADDRESS[TX_ADR_WIDTH]  = {0x34,0x43,0x10,0x10,0x01}; // Define a static TX address
unsigned char 	Buffer[]={
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
};
unsigned char 	RX_Buffer[]={
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
};
void delay(void)//
{
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	__NOP();
}
void delayms(unsigned short	dly)//
{
	for(;dly>0;dly--) ;
}

void clear_buf(unsigned char	*ptr,unsigned char number)//??buffer,??????
{
	for (;number>0;number--) *ptr++=0;
}
//------------------------------------------------------
/**************************************************
Function: SPI_RW();

Description:
  Writes one byte to nRF24L01, and return the byte read
  from nRF24L01 during write, according to SPI protocol  */
/**************************************************/
unsigned char SPI_RW(unsigned char byte)
{
	unsigned char rx_byte;
/*	unsigned char  bit_ctr;	
   	for(bit_ctr=0;bit_ctr<8;bit_ctr++)   	// output 8-bit
   	{
   		if(byte & 0x80) 
   			nRF24L01_MOSIH;
   		else 
   			nRF24L01_MOSIL;
		delay();
   		byte = (byte << 1);           		// shift next bit into MSB..
   		nRF24L01_SCKH;
		delay();
 		if(PINB&0x08) byte |= 1;
   		nRF24L01_SCKL;
		delay();
   	}*/
		HAL_SPI_TransmitReceive(&hspi1, &byte, &rx_byte, 1,HAL_MAX_DELAY);
    return(rx_byte);           		  		// return read byte
}
/**************************************************
Function: SPI_RW_Reg();

Description:
  Writes value 'value' to register 'reg' */
/**************************************************/
unsigned char SPI_RW_Reg(unsigned char reg, unsigned char value)
{
	unsigned char status;
	nRF24L01_CSNL;
	delay();
	status = SPI_RW(reg);      // select register
  	SPI_RW(value);             // ..and write value to it..
	delay();
	nRF24L01_CSNH;
	delay();
  	return(status);            // return nRF24L01 status byte
}
/**************************************************
Function: SPI_Read();

Description:
  Read one byte from nRF24L01 register, 'reg'  */
/**************************************************/
unsigned char SPI_Read(unsigned char reg)
{
	unsigned char reg_val;
	nRF24L01_CSNL;
	delay();
  	SPI_RW(reg);            // Select register to read from..
  	reg_val = SPI_RW(0);    // ..then read registervalue
	delay();
	nRF24L01_CSNH;
	delay();
  	return(reg_val);        // return register value
}
/**************************************************
Function: SPI_Read_Buf();

Description:
  Reads 'bytes' #of bytes from register 'reg'
  Typically used to read RX payload, Rx/Tx address */
/**************************************************/
unsigned char SPI_Read_Buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes)
{
	unsigned char status,byte_ctr;
	nRF24L01_CSNL;
	delay();
  	status = SPI_RW(reg);       		// Select register to write to and read status byte
	for(byte_ctr=0;byte_ctr<bytes;byte_ctr++)
    	pBuf[byte_ctr] = SPI_RW(0);    // Perform SPI_RW to read byte from nRF24L01
	nRF24L01_CSNH;
	delay();
  	return(status);                    // return nRF24L01 status byte
}
/**************************************************
Function: SPI_Write_Buf();

Description:
  Writes contents of buffer '*pBuf' to nRF24L01
  Typically used to write TX payload, Rx/Tx address */
/**************************************************/
unsigned char SPI_Write_Buf(unsigned char reg, unsigned char *pBuf, unsigned char bytes)
{
	unsigned char status,byte_ctr;
	nRF24L01_CSNL;                   		// Set nRF24L01_CSN low, init SPI tranaction
  	delay();
	status = SPI_RW(reg);    					// Select register to write to and read status byte
  	for(byte_ctr=0; byte_ctr<bytes; byte_ctr++) // then write all byte in buffer(*pBuf)
    	status = SPI_RW(*pBuf++);            
	nRF24L01_CSNH; 								// Set nRF24L01_CSN high again
	delay();
  	return(status);          					// return nRF24L01 status byte
}
/**************************************************
Function: RX_Mode();

Description:
  This function initializes one nRF24L01 device to
  RX Mode, set RX address, writes RX payload width,
  select RF channel, datarate & LNA HCURR.
  After init, CE is toggled high, which means that
  this device is now ready to receive a datapacket. */
/**************************************************/
void RX_Mode(void)
{
	nRF24L01_CEL;
	delay();
  	SPI_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // Use the same address on the RX device as the TX device

  	SPI_RW_Reg(NRF_WRITE_REG + EN_AA, 0x01);      // Enable Auto.Ack:Pipe0
  	SPI_RW_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);  // Enable Pipe0
  	SPI_RW_Reg(NRF_WRITE_REG + RF_CH, 40);        // Select RF channel 40
  	SPI_RW_Reg(NRF_WRITE_REG + RX_PW_P0, TX_PLOAD_WIDTH); // Select same RX payload width as TX Payload width
  	SPI_RW_Reg(NRF_WRITE_REG + RF_SETUP, 0x07);   // TX_PWR:0dBm, Datarate:2Mbps, LNA:HCURR
  	SPI_RW_Reg(NRF_WRITE_REG + CONFIG, 0x0f);     // Set PWR_UP bit, enable CRC(2 bytes) & Prim:RX. RX_DR enabled..
  	nRF24L01_CEH;
	delay();
	//  This device is now ready to receive one packet of 16 bytes payload from a TX device sending to address
  //  '3443101001', with auto acknowledgment, retransmit count of 10, RF channel 40 and datarate = 2Mbps.

}
/**************************************************/

/**************************************************
Function: TX_Mode();

Description:
  This function initializes one nRF24L01 device to
  TX mode, set TX address, set RX address for auto.ack,
  fill TX payload, select RF channel, datarate & TX pwr.
  PWR_UP is set, CRC(2 bytes) is enabled, & PRIM:TX.

  ToDo: One high pulse(>10us) on CE will now send this
  packet and expext an acknowledgment from the RX device. */
/**************************************************/
void TX_Mode(void)
{
	nRF24L01_CEL;
	delay();
  	SPI_Write_Buf(NRF_WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // Writes TX_Address to nRF24L01
  	SPI_Write_Buf(NRF_WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // RX_Addr0 same as TX_Adr for Auto.Ack
  	SPI_Write_Buf(NRF_WR_TX_PLOAD, Buffer, TX_PLOAD_WIDTH); // Writes data to TX payload

  	SPI_RW_Reg(NRF_WRITE_REG + EN_AA, 0x01);      // Enable Auto.Ack:Pipe0
  	SPI_RW_Reg(NRF_WRITE_REG + EN_RXADDR, 0x01);  // Enable Pipe0
  	SPI_RW_Reg(NRF_WRITE_REG + SETUP_RETR, 0x1a); // 500us + 86us, 10 retrans...
  	SPI_RW_Reg(NRF_WRITE_REG + RF_CH, 40);        // Select RF channel 40
  	SPI_RW_Reg(NRF_WRITE_REG + RF_SETUP, 0x07);   // TX_PWR:0dBm, Datarate:2Mbps, LNA:HCURR
  	SPI_RW_Reg(NRF_WRITE_REG + CONFIG, 0x0e);     // Set PWR_UP bit, enable CRC(2 bytes) & Prim:TX. MAX_RT & TX_DS enabled..
	nRF24L01_CEH;
	delay();
	
}
//------------------------------------------------------
