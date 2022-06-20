#ifndef __LCD_INTERFACE_HPP
#define __LCD_INTERFACE_HPP
#include "stdint.h"
#include "stdlib.h"
#include <stdio.h>
#include "Port.h"
#include "SysTick.h"
#include "ssd1306xled.h"
#ifdef __cplusplus
extern "C++" {
#endif
#define STR_LENGTH 0x40
enum ScreenSize
{
	Screen128x64 = 0
};
const uint8_t ScreenSizeList[][2] = {
	{128, 64}
};
enum FontSize
{
	Font6x8 = 0,
	Font8x16 = 1
};
const uint8_t FontSizeList[][2] = {
	{6, 8},
	{8, 16}
};
enum TextAlignment
{
	Left,
	Right,
	Center,
	Stretch,
	Inherent
};

class LCDIF;
//MANIPULATORS
class manip {
public:
		manip();
		virtual void act(LCDIF* lcd);
		virtual ~manip(){}
};

//Center of the Line
class center: public manip {//prints to the center of the screen at specific height
public:
		uint8_t vpos;
		center(uint8_t y){
			vpos = y;
		}
		void act(LCDIF* lcd);
};

class pos:public manip {//prints to specific position
public:
		uint8_t hpos;
		uint8_t vpos;
		pos(uint8_t x, uint8_t y):manip(){
			hpos = x;
			vpos = y;
		}
		void act(LCDIF* lcd);
};
class left:public manip{//prints to the beginning of the line at specific height
public:
		uint8_t hpos;
		uint8_t vpos;
		left(uint8_t y, uint8_t x = 0):manip(){
			vpos = y;
			hpos = x;
		}
		void act(LCDIF* lcd);
};
class right:public manip{//prints to the end of the line
public:
		uint8_t hpos;
		uint8_t vpos;
		right(uint8_t y, uint8_t x = 0):manip(){
			vpos = y;
			hpos = x;
		}
		void act(LCDIF* lcd);
};

class stretch:public manip{//prints to address
public:
		int8_t vpos;
		int8_t start;
		int8_t end;
		stretch(int8_t y, int8_t x0 = 0, int8_t x1 = 0):manip(){
			vpos = y;
			start = x0;
			end = x1;
		}
		void act(LCDIF* lcd);
};

//Wait N milliseconds
class wait:public manip{//wait n ms
public:
		int ms;
		wait(int msec):manip(){
			ms = msec;
		}
		void act(LCDIF* lcd);
};

//Set number of digits after decimal point in DOUBLE
class precision:public manip{
public:
		uint8_t nod;
		precision(uint8_t num):manip(){
			nod = num;
		}
		void act(LCDIF* lcd);
};
class font:public manip{
public:
		FontSize f;
		font(FontSize size):manip(){
			f = size;
		}
		void act(LCDIF* lcd);
};
class alignment:public manip{
public:
		TextAlignment align;
		alignment(TextAlignment al):manip(){
			align = al;
		}
		void act(LCDIF* lcd);
};
class update:public manip{
public:
		bool clear_buf;
		update(bool clear_buffer = true):manip(){
			clear_buf = clear_buffer;
		}
		void act(LCDIF* lcd);
};
//Print Symbol
class symbol:public manip{
public:
		uint8_t code;
		symbol(uint8_t c):manip(){
			code = c;
		}
		void act(LCDIF* lcd);
};
class text_mode:public manip{
public:
		bool en;
		bool wrap;
		uint8_t hori_offset;
		TextAlignment alignment;

		text_mode(bool enable, bool wrapping, TextAlignment align = Left, uint8_t h_off = 0):manip(){
			en = enable;
			hori_offset = h_off;
			alignment = align;
			wrap = wrapping;
		}
		void act(LCDIF* lcd);
};
class line:public manip{
public:
		int8_t line_num;
		TextAlignment alignment;
		line(int8_t line = -1, TextAlignment align = Inherent):manip(){
			line_num = line;
			alignment = align;
		}
		void act(LCDIF* lcd);
};
//Clear Screen And Buffer
void clr(LCDIF* lcd);

//Class for LCD
class LCDIF{

private:
		SSD1306Device 	m_LcdDevice;
		ScreenSize 			m_ScreenSize;
		FontSize 				m_CurrentFontSize;
		
		//int 						m_Leading;//min value is 1
		int 						m_Spacing;//min value is 1
		uint8_t					m_Precision;//for double

		bool 						m_TextMode;
		//uint8_t 				m_TopOffset;
		uint8_t 				m_SideOffset;
		TextAlignment		m_TextAlignment;

		uint8_t					m_CurX;
		uint8_t					m_CurY;
		
		//int 						m_CurLine;
		bool 						m_WordWrapping;

		char						m_CurStr[0x40];
		char						m_InnerStr[0x40];//working str (grinar) 31.07.20
		uint8_t					m_CharCount;
public:
		LCDIF();  //default parameters
		LCDIF(ScreenSize size);
		LCDIF(const LCDIF &other);
		LCDIF& operator=(const LCDIF &other);
		~LCDIF();

		void InitLowLevel();

void PrintS2Pos(char*,uint8_t);
private:
		void PrintString(char*);
		void PrintChar(char);

		void Home();
		void PrintLine(char* s, uint8_t vpos = 0, TextAlignment alignment = Left, uint8_t hpos_left = 0, uint8_t hpos_right = 0);
		void PrintLine(uint8_t vpos = 0, TextAlignment alignment = Left, uint8_t hpos_left = 0, uint8_t hpos_right = 0);
		void PrintText(int line, TextAlignment align);
public:
		//void SetPrecision(uint8_t num);
		//void SetSpacing(int num);
		//void SetLeading(int num);
		//void SetFont(FontSize size);
		//void SetAlignment(TextAlignment alignment);
		void ClearBuffer();
		void ClearScreen();

		//MANIPULATORS + clr
		LCDIF&operator<<(const char& c){
				char a[2] = {c,'\0'};
				strCat(m_CurStr, a, m_CharCount);
				return *this;
		}
		
		LCDIF&operator<<(char* s){
				strCat(m_CurStr, fromUTF8(s), m_CharCount);
				return *this;
		}
		
		LCDIF&operator<<(const char* s){
				strCat(m_CurStr, fromUTF8(const_cast<char*>(s)), m_CharCount);
				return *this;
		}
		
		LCDIF&operator<<(const int& i){
					strCat(m_CurStr, strInt(i), m_CharCount);
					return *this;
		}
		
		LCDIF&operator<<(const double& i){
					strCat(m_CurStr, strDbl(i, m_Precision), m_CharCount);
					return *this;
		}
		LCDIF&operator<<(pos man){
					man.act(this);
					return *this;
		}
		LCDIF&operator<<(left man){
					man.act(this);
					return *this;
		}
		LCDIF&operator<<(right man){
					man.act(this);
					return *this;
		}
		LCDIF&operator<<(center man){
					man.act(this);
					return *this;
		}
		LCDIF&operator<<(wait man){
					man.act(this);
					return *this;
		}
		LCDIF&operator<<(stretch man){
					man.act(this);
					return *this;
		}
		LCDIF&operator<<(precision man){
					man.act(this);
					return *this;
		}
		LCDIF&operator<<(font man){
					man.act(this);
					return *this;
		}
		LCDIF&operator<<(text_mode man){
					man.act(this);
					return *this;
		}
		LCDIF&operator<<(line man){
					man.act(this);
					return *this;
		}
		LCDIF&operator<<(symbol man){
					man.act(this);
					return *this;
		}
		LCDIF&operator<<(update man){
					man.act(this);
					return *this;
		}
		LCDIF&operator<<(void (*ff)(LCDIF*)){
					(*ff)(this);
					return *this;
		}

		friend class pos;
		friend class left;
		friend class right;
		friend class wait;
		friend class center;
		friend class stretch;
		
		friend class symbol;
		friend class precision;
		
		friend class font;
		friend class text_mode;
		friend class line;
		
		friend class update;
		
		
private: //by PMA on 30/07/2020
		char* 	fromUTF8(char* str);//by 

		uint8_t strLen(char* s);
		char* 	strSub(char* s, uint8_t start, uint8_t end);
		void		strCat(char* dst, char* src, uint8_t &dstlen);
		char*		strDbl(double val, uint8_t precision);
		char*		strInt(int val);

		char* 	getNextLine(char* dst, int& offset);
};
#ifdef __cplusplus
}
#endif
#endif /* __LCD_INTERFACE_H */
