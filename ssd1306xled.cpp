/*
 * SSD1306xLED - Drivers for SSD1306 controlled dot matrix OLED/PLED 128x64 displays
 *
 * @created: 2014-08-12
 * @author: Neven Boyanov
 *
 * Source code available at: https://bitbucket.org/tinusaur/ssd1306xled
 * 
 * Modified by Tejashwi Kalp Taru, with the help of TinyI2C (https://github.com/technoblogy/tiny-i2c/)
 * Modified code available at: https://github.com/tejashwikalptaru/ssd1306xled
 */

// ----------------------------------------------------------------------------


#include <stdlib.h>

#include "ssd1306xled.h"
#include "font6x8.h"
#include "font8x16.h"

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"

#include "SysTick.h"
#include <string.h>
// ----------------------------------------------------------------------------

// Some code based on "IIC_wtihout_ACK" by http://www.14blog.com/archives/1358

const uint8_t ssd1306_init_sequence [] = {	// Initialization Sequence
	0xAE,			// Set Display ON/OFF - AE=OFF, AF=ON
	0xD5, 0xF0,		// Set display clock divide ratio/oscillator frequency, set divide ratio
	0xA8, 0x3F,		// Set multiplex ratio (1 to 64) ... (height - 1)
	0xD3, 0x00,		// Set display offset. 00 = no offset
	0x40 | 0x00,	// Set start line address, at 0.
	0x8D, 0x14,		// Charge Pump Setting, 14h = Enable Charge Pump
	0x20, 0x00,		// Set Memory Addressing Mode - 00=Horizontal, 01=Vertical, 10=Page, 11=Invalid
	0xA0 | 0x01,	// Set Segment Re-map
	0xC8,			// Set COM Output Scan Direction
	0xDA, 0x12,		// Set COM Pins Hardware Configuration - 128x32:0x02, 128x64:0x12
	0x81, 0x3F,		// Set contrast control register
	0xD9, 0x22,		// Set pre-charge period (0x22 or 0xF1)
	0xDB, 0x20,		// Set Vcomh Deselect Level - 0x00: 0.65 x VCC, 0x20: 0.77 x VCC (RESET), 0x30: 0.83 x VCC
	0xA4,			// Entire Display ON (resume) - output RAM to display
	0xA6,			// Set Normal/Inverse Display mode. A6=Normal; A7=Inverse
	0x2E,			// Deactivate Scroll command
	0xAF,			// Set Display ON/OFF - AE=OFF, AF=ON
};

// Program:    5248 bytes

SSD1306Device::SSD1306Device(void){}

void SSD1306Device::I2CInit() {
	I2C_Device = I2C1;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  DISABLE);
	
	I2C_Cmd(I2C_Device, DISABLE);
  I2C_DeInit(I2C_Device);
	
	I2C_InitStruct.I2C_ClockSpeed = 400000; 
  I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStruct.I2C_OwnAddress1 = 0;//SSD1306_SA;
  I2C_InitStruct.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_Init(I2C_Device, &I2C_InitStruct);
	
	//GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  //GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD; 
  //GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  //GPIO_Init(GPIOB, &GPIO_InitStruct);
  //GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
  //GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);
	
  I2C_Cmd(I2C_Device, ENABLE);
	I2C_AcknowledgeConfig(I2C_Device, ENABLE);
	
	posX = 0;
	posY = 0;
	memset(ScreenBuffer, 0, 0x400);
}

bool SSD1306Device::I2CStart(uint8_t address, int readcount) {
	if (readcount != 0) { I2Ccount = readcount; readcount = 1; }
	uint8_t addressRW = address<<1 | readcount;

	while(I2C_GetFlagStatus(I2C_Device, I2C_FLAG_BUSY));
	I2C_GenerateSTART(I2C_Device, ENABLE);
	while(!I2C_CheckEvent(I2C_Device, I2C_EVENT_MASTER_MODE_SELECT));
	I2C_Send7bitAddress(I2C_Device, addressRW, I2C_Direction_Transmitter);
	while(!I2C_CheckEvent(I2C_Device, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	return true;                                // Start successfully completed
}

void SSD1306Device::I2CStop (void) {
  I2C_GenerateSTOP(I2C_Device, ENABLE);
	while (I2C_GetFlagStatus(I2C_Device, I2C_FLAG_STOPF));
}

void SSD1306Device::begin() {
	I2CInit();
	while (!I2CStart(SSD1306_SA, 0)) {
		DelayMs(10);
	}
	I2CStop();
}

bool SSD1306Device::I2CWrite(uint8_t data)  {
	I2C_SendData(I2C_Device, data);
  while(!I2C_CheckEvent(I2C_Device, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  return true;
}

void SSD1306Device::ssd1306_init(void)
{
	begin();
	ssd1306_send_command_start();
	for (uint8_t i = 0; i < sizeof (ssd1306_init_sequence); i++) {
		ssd1306_send_byte(ssd1306_init_sequence[i]);
	}
	ssd1306_send_command_stop();
	ssd1306_fillscreen(0);
}

void SSD1306Device::ssd1306_send_command_start(void) {
	//I2CStop();
	I2CStart(SSD1306_SA, 0);
	I2CWrite(SSD1306_COMMAND);
}

void SSD1306Device::ssd1306_send_command_stop() {
	I2CStop();
}

void SSD1306Device::ssd1306_send_command(uint8_t command) {
	ssd1306_send_command_start();
	ssd1306_send_byte(command);
	ssd1306_send_command_stop();
}

void SSD1306Device::ssd1306_send_byte(uint8_t byte) {
	I2CWrite(byte);
}

void SSD1306Device::ssd1306_send_data_start(void) {
	//I2CStop();
	I2CStart(SSD1306_SA, 0);
	I2CWrite(SSD1306_DATA);
}

void SSD1306Device::ssd1306_send_data_stop() {
	I2CStop();
}

void SSD1306Device::ssd1306_fillscreen(uint8_t fill) {
	ssd1306_setpos(0, 0, true);
	ssd1306_send_data_start();	// Initiate transmission of data
	for (uint16_t i = 0; i < 256; i++) {
		I2CWrite(fill);
		I2CWrite(fill);
		I2CWrite(fill);
		I2CWrite(fill);
	}
	ssd1306_send_data_stop();	// Finish transmission
}

void SSD1306Device::ssd1306_setpos(uint8_t x, uint8_t y, bool update)
{
	if (!update)
	{
		posX = x;
		posY = y;
	}
	else 
	{
		ssd1306_send_command_start();
		ssd1306_send_byte(0xb0 | (y & 0x07));
		ssd1306_send_byte(0x10 | ((x & 0xf0) >> 4));
		ssd1306_send_byte(x & 0x0f); // | 0x01
		ssd1306_send_command_stop();
	}
}

void SSD1306Device::ssd1306_char_font6x8(char ch) {
	int c = (ch - 32)*6;
	int pos = (128 * posY) + (posX);
	for (int i = 0; i < 6; i++)
		ScreenBuffer[pos + i] = (ssd1306xled_font6x8[c + i]);	
	/*uint8_t i; 
	uint8_t c = ch - 32;
	ssd1306_send_data_start();
	for (i = 0; i < 6; i++)
	{
		ssd1306_send_byte(ssd1306xled_font6x8[c * 6 + i]);
	}
	ssd1306_send_data_stop();*/
}
void SSD1306Device::ssd1306_string_font6x8(char *s) {
	while (*s)
	{
		ssd1306_char_font6x8(*s++);
		posX+=6;
	}
}

void SSD1306Device::ssd1306_draw_bmp(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t bitmap[]){
	/*uint16_t j = 0;
	uint8_t y, x;
	if (y1 % 8 == 0) y = y1 / 8;
	else y = y1 / 8 + 1;
	for (y = y0; y < y1; y++)
	{
		ssd1306_setpos(x0,y);
		ssd1306_send_data_start();
		for (x = x0; x < x1; x++)
		{
			ssd1306_send_byte(bitmap[j++]);
		}
		ssd1306_send_data_stop();
	}*/
}

void SSD1306Device::ssd1306_char_f8x16(uint8_t x, uint8_t y, const char ch[])
{
	posX = x;
	posY = y;
	int pos = (128 * posY) + posX;
	int j = 0, i = 0;
	while (ch[j] != '\0')
	{
		int c = (ch[j] - 32)*16;
		pos = 128 * posY + posX;
		for (i = 0; i < 8; i++)
			ScreenBuffer[pos + i] = (ssd1306xled_font8x16[c + i]);
		pos = 128 * (posY + 1) + posX;
		for (i = 0; i < 8; i++)
			ScreenBuffer[pos + i] = (ssd1306xled_font8x16[c + i + 8]);
		posX += 8;
		j++;
	}
	/*int j = 0, i = 0;
	while (ch[j] != '\0')
	{
		uint8_t c = ch[j] - 32;
		if (x > 120)
		{
			x = 0;
			y++;
		}
		ssd1306_setpos(x, y);
		ssd1306_send_data_start();
		for (i = 0; i < 8; i++)
		{
			ssd1306_send_byte(ssd1306xled_font8x16[c * 16 + i]);
		}
		ssd1306_send_data_stop();
		ssd1306_setpos(x, y + 1);
		ssd1306_send_data_start();
		for (i = 0; i < 8; i++)
		{
			ssd1306_send_byte(ssd1306xled_font8x16[c * 16 + i + 8]);
		}
		ssd1306_send_data_stop();
		x += 8;
		j++;
	}*/
}
void SSD1306Device::ssd1306_update(bool clear_after)
{
	for (uint16_t j = 0; j < 8; j++)
	{
		ssd1306_setpos(0, j, true);
		ssd1306_send_data_start();
		for (uint16_t i = 0; i < 0x80; i++)
			I2CWrite(ScreenBuffer[j * 0x80 + i]);
		ssd1306_send_data_stop();
	}
	if (clear_after)
		memset(ScreenBuffer, 0, 0x400);
}

// ----------------------------------------------------------------------------