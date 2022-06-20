#ifndef __CANPROTOCOL_H
#define __CANPROTOCOL_H

typedef unsigned int uint32_t;

const uint32_t ID_MAIN        = 0xA000;
const uint32_t ID_DRIVER0     = 0xB000;
const uint32_t ID_DRIVER1     = 0xB001;
const uint32_t ID_DRIVER2     = 0xB002;
const uint32_t ID_DRIVER3     = 0xB003;
const uint32_t ID_DRIVER4     = 0xB004;
const uint32_t ID_DRIVER5     = 0xB005;
const uint32_t ID_DRIVER6     = 0xB006;
const uint32_t ID_DRIVER7     = 0xB007;
const uint32_t ID_DRIVER8     = 0xB008;
const uint32_t ID_DRIVER9     = 0xB009;

const uint32_t COMMAMD_CHECK0 = 0x0010;
const uint32_t COMMAMD_CHECK1 = 0x0011;
const uint32_t REPLY_CHECK0   = 0x1010;
const uint32_t REPLY_CHECK1   = 0x1011;

const uint32_t COMMAMD_RESET  = 0x0020;

const uint32_t COMMAMD_PUSH0 = 0x00A0;
const uint32_t COMMAMD_PULL0 = 0x00A1;
const uint32_t COMMAMD_PUSH  = 0x00A2;
const uint32_t COMMAMD_PUSH1 = 0x00B0;
const uint32_t COMMAMD_PULL1 = 0x00B1;	
const uint32_t COMMAMD_PULL  = 0x00B2;
	
const uint32_t REPLY_PUSHOK0 = 0x10A0;
const uint32_t REPLY_PULLOK0 = 0x10A1;
const uint32_t REPLY_PUSHOK1 = 0x10B0;
const uint32_t REPLY_PULLOK1 = 0x10B1;

const uint32_t PUSHPULLDRIVER0 = 0xA000;
const uint32_t PUSHPULLDRIVER1 = 0xA001;
#endif