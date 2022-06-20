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
 *
 */
#include <stdint.h>
#include "stm32f10x_i2c.h"
#include "stm32f10x_gpio.h"

#ifndef SSD1306XLED_H
#define SSD1306XLED_H

#ifndef SSD1306_SA
#define SSD1306_SA		0x3C	// Slave address
#endif
// Constants
// Prepare register value to: Clear flags, and set USI to shift 8 bits i.e. count 16 clock edges.
//const unsigned char USISR_8bit = 1<<USISIF | 1<<USIOIF | 1<<USIPF | 1<<USIDC | 0x0<<USICNT0;
// Prepare register value to: Clear flags, and set USI to shift 1 bit i.e. count 2 clock edges.
//const unsigned char USISR_1bit = 1<<USISIF | 1<<USIOIF | 1<<USIPF | 1<<USIDC | 0xE<<USICNT0;

#define SSD1306_COMMAND 0x00
#define SSD1306_DATA 0x40

// ----------------------------------------------------------------------------

class SSD1306Device
{
    public:
		SSD1306Device(void);
		void ssd1306_init(void);

		void ssd1306_send_data_start(void);
		void ssd1306_send_data_stop(void);
		void ssd1306_send_byte(uint8_t byte);

		void ssd1306_send_command_start(void);
		void ssd1306_send_command_stop(void);
		void ssd1306_send_command(uint8_t command);

		void ssd1306_setpos(uint8_t x, uint8_t y, bool update = false);
		void ssd1306_fillscreen(uint8_t fill);
		void ssd1306_char_font6x8(char ch);
		void ssd1306_string_font6x8(char *s);
		void ssd1306_char_f8x16(uint8_t x, uint8_t y, const char ch[]);
		
		void ssd1306_draw_bmp(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t bitmap[]);

		void ssd1306_update(bool clear_after = true);//05.11.2020 (grinar)
	private:
		int I2Ccount;
		void I2CInit();
		bool I2CStart(uint8_t address, int readcount);
		uint8_t I2CTransfer (uint8_t data);
		void I2CStop (void);
		bool I2CWrite(uint8_t data);
		void begin();
	
	private://stm std periph lib instances (grinar) 14.10.2020
		I2C_TypeDef* I2C_Device;
		I2C_InitTypeDef I2C_InitStruct;
		GPIO_InitTypeDef GPIO_InitStruct;
	
		char ScreenBuffer[0x400];
		int posX;
		int posY;
};

// ----------------------------------------------------------------------------

#endif
