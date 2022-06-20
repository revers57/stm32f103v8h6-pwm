#include "LCDIF.h"
#ifdef __cplusplus
extern "C++" {
#endif
manip::manip(){}
void manip::act(LCDIF* lcd){}
//default constructor
	LCDIF::LCDIF() : m_ScreenSize(Screen128x64), m_CurrentFontSize(Font6x8), m_Spacing(1), m_Precision(5){
		m_CurStr[0]=0;		//Clear Buffer
}
LCDIF::LCDIF(ScreenSize size) : m_ScreenSize(size), m_CurrentFontSize(Font6x8), m_Spacing(1), m_Precision(5){
		m_CurStr[0]=0;		//Clear Buffer
}
//destructor
LCDIF::~LCDIF(){
}
LCDIF::LCDIF(const LCDIF &other){
	m_CurStr[0]=0;		//Clear Buffer
	m_Precision = other.m_Precision;
	m_ScreenSize = other.m_ScreenSize;
	m_Spacing = other.m_Spacing;
	m_CurrentFontSize = other.m_CurrentFontSize;
}
LCDIF& LCDIF::operator=(const LCDIF &other){
	m_CurStr[0]=0;		//Clear Buffer
	m_Precision = other.m_Precision;
	m_ScreenSize = other.m_ScreenSize;
	m_Spacing = other.m_Spacing;
	m_CurrentFontSize = other.m_CurrentFontSize;
	return *this;
}
void LCDIF::InitLowLevel()
{
	 DelayMs(40);
   m_LcdDevice.ssd1306_init();
}
void LCDIF::ClearScreen(){
	 m_LcdDevice.ssd1306_fillscreen(0);
}
void LCDIF::PrintString(char* s){
	switch(m_CurrentFontSize)
	{
		case Font6x8:
			if (m_Spacing == 1)
			{
				m_LcdDevice.ssd1306_setpos(m_CurX, m_CurY);
				m_LcdDevice.ssd1306_string_font6x8(s);
			}
			else
			{
				while(*s)
				{
					m_LcdDevice.ssd1306_setpos(m_CurX, m_CurY);
					m_LcdDevice.ssd1306_char_font6x8(*s++);
					m_CurX += (FontSizeList[m_CurrentFontSize][0] + m_Spacing - 1);
				}
			}
			break;
		case Font8x16:
			if (m_Spacing == 1)
			{
				m_LcdDevice.ssd1306_char_f8x16(m_CurX, m_CurY, s);
			}
			else
			{
				while(*s)
				{
					char arr[2] = {*s++, 0};
					m_LcdDevice.ssd1306_char_f8x16(m_CurX, m_CurY, arr);
					m_CurX += (FontSizeList[m_CurrentFontSize][0] + m_Spacing - 1);
				}
			}
			break;
	}
}
void LCDIF::PrintChar(char c){
	switch(m_CurrentFontSize)
	{
		case Font6x8:
			m_LcdDevice.ssd1306_setpos(m_CurX, m_CurY);
			m_LcdDevice.ssd1306_char_font6x8(c);
			m_CurX += (FontSizeList[m_CurrentFontSize][0] + m_Spacing - 1);
			break;
		case Font8x16:
			{
				char arr[2] = {c, 0};
				m_LcdDevice.ssd1306_char_f8x16(m_CurX, m_CurY, arr);
				m_CurX += (FontSizeList[m_CurrentFontSize][0] + m_Spacing - 1);
			}
			break;
	}
}

void LCDIF::Home(){
	if (m_TextMode)
	{
		m_CurY = 0;
		m_CurX = m_SideOffset;
	}
	else
	{
		m_CurY = 0;
		m_CurX = 0;
	}
}
void LCDIF::PrintLine(char* s, uint8_t vpos, TextAlignment alignment, uint8_t hpos_left, uint8_t hpos_right){
	m_CurY = vpos; 
	int old_spacing = m_Spacing;
	uint8_t width = ScreenSizeList[m_ScreenSize][0] - hpos_left - hpos_right;
	uint8_t str_len = strLen(s);
	uint8_t str_width = str_len*FontSizeList[m_CurrentFontSize][0] + (str_len - 1)*(m_Spacing - 1);
	m_Spacing = (alignment == Stretch) ? 1 : m_Spacing;
	if (str_width > width)
	{
		uint8_t charToCut;
		switch(alignment)
		{
		case Center:
			charToCut = ((str_width - width)/(FontSizeList[m_CurrentFontSize][0] + m_Spacing - 1));
			s = strSub(s, charToCut/2, str_len - (charToCut/2));
			str_len -= charToCut;
			str_width = str_len*FontSizeList[m_CurrentFontSize][0] + (str_len - 1)*(m_Spacing - 1);
			break;
		case Left:
			charToCut = ((str_width - width)/(FontSizeList[m_CurrentFontSize][0] + m_Spacing - 1));
			s = strSub(s, 0, str_len - charToCut);
			str_len -= charToCut;
			break;
		case Right:
			charToCut = ((str_width - width)/(FontSizeList[m_CurrentFontSize][0] + m_Spacing - 1));
			s = strSub(s, charToCut, str_len - charToCut);
			str_len -= charToCut;
			str_width = str_len*FontSizeList[m_CurrentFontSize][0] + (str_len - 1)*(m_Spacing - 1);
			break;
		case Stretch:
			charToCut = ((str_width - width)/(FontSizeList[m_CurrentFontSize][0]));
			s = strSub(s, 0, str_len - charToCut);
			str_len -= charToCut;
			break;
		}
	}
	switch(alignment)
	{
	case Center:
		m_CurX = hpos_left + (width - str_width)/2;
		break;
	case Left:
		m_CurX = hpos_left;
		break;
	case Right:
		m_CurX = hpos_left + width - str_width - hpos_right;
		break;
	case Stretch:
		m_CurX =  hpos_left;
		m_Spacing = (width - (FontSizeList[m_CurrentFontSize][0]*str_len))/(str_len - 1) + 1;
		break;
	}
	PrintString(s);
	m_Spacing = old_spacing;
}
void LCDIF::PrintLine(uint8_t vpos, TextAlignment alignment, uint8_t hpos_left, uint8_t hpos_right){
	PrintLine(m_CurStr, vpos, alignment, hpos_left, hpos_right);
}
void LCDIF::PrintText(int line, TextAlignment alignment)
{
	if (line!=-1)
		m_CurY = line;
	TextAlignment align = alignment == Inherent? m_TextAlignment : alignment;
	
	int offset = 0;
	while (offset!=-1)
	{
		PrintLine(getNextLine(m_CurStr, offset), m_CurY,
										align, m_SideOffset, m_SideOffset);
		m_CurY += FontSizeList[m_CurrentFontSize][1]/8;
	}
}
char* LCDIF::fromUTF8(char* s){
	char c;
	int i = 0;
	while ((c = *s)) {
		if (c <= 127) //ASCII range
			m_InnerStr[i] = c;
		//Shift key for cyrillic (utf8) (grinar) 15.10.2020
		else if (c == 0xD0)
		{
			c = *(++s);//get key to be shifted
			if (c == 0x81) // Ё
					m_InnerStr[i] = 191;
			else
					m_InnerStr[i] = c - 16;
		}
		else if (c == 0xD1)
		{
			c = *(++s);//get key to be shifted
			if (c == 0x91) // Ё
					m_InnerStr[i] = 192;
			else
					m_InnerStr[i] = c + 48;
		}
		s++;
		i++;
	}
	m_InnerStr[i]='\0';
	return m_InnerStr;
}

uint8_t LCDIF::strLen(char* s)
{
	uint8_t len = 0;
	while (*s++!=0)
		len++;
	return len;
}
char* LCDIF::strSub(char* s, uint8_t start, uint8_t end)
{
	int i = start;
	for (; i < end; i++)
		m_InnerStr[i-start] = s[i];
	m_InnerStr[i-start] = 0;
	return m_InnerStr;
}
void LCDIF::strCat(char* dst, char* src, uint8_t &dstlen)
{
	while(*src)
	{
		dst[dstlen++] = *src++;
	}
	dst[dstlen] = 0;
}
char*	LCDIF::strDbl(double val, uint8_t precision)
{
	int sign = 0;
	if (val<0)
	{
		sign = 1;
		m_InnerStr[0] = '-';
	}
	int count = 1;
	int val_int = static_cast<int>(val);
	while((val_int/=10)>0) count++;
	val_int = static_cast<int>(val);
	int i = count - 1;
	for (; i >= 0; i--)
	{
		m_InnerStr[i + sign] = (val_int%10) + '0';
		val_int/=10;
	}
	m_InnerStr[count + sign] = ',';
	val -= val_int;
	for (i = 0; i < precision; i++)
		val*=10;
	val_int = static_cast<int>(val);
	i = precision;
	for (; i > 0; i--)
	{
		m_InnerStr[i + count + sign] = (val_int%10) + '0';
		val_int/=10;
	}
	m_InnerStr[precision + count + sign + 1] = 0;
	return m_InnerStr;
}
char*	LCDIF::strInt(int val)
{
	int sign = 0;
	if (val<0)
	{
		sign = 1;
		m_InnerStr[0] = '-';
	}
	int count = 1;
	int val_int = static_cast<int>(val);
	while((val_int/=10)>0) count++;
	val_int = static_cast<int>(val);
	int i = count - 1;
	for (; i >= 0; i--)
	{
		m_InnerStr[i + sign] = (val_int%10) + '0';
		val_int/=10;
	}
	m_InnerStr[count + sign] = 0;
	return m_InnerStr;
}

void LCDIF::ClearBuffer(){
	m_CurStr[0] = 0;
	//m_InnerStr[0] = 0;
	m_CharCount = 0;
}

void center::act(LCDIF* lcd){
	lcd->PrintLine(lcd->m_CurStr, vpos, Center);
	lcd->ClearBuffer();
}
void pos::act(LCDIF* lcd){
	lcd->PrintLine(lcd->m_CurStr, vpos, Left, hpos);
	lcd->ClearBuffer();
}

void left::act(LCDIF* lcd){
	lcd->PrintLine(lcd->m_CurStr, vpos, Left, hpos);
	lcd->ClearBuffer();
}

void right::act(LCDIF* lcd){
	lcd->PrintLine(lcd->m_CurStr, vpos, Right, 0, hpos);
	lcd->ClearBuffer();
}

void stretch::act(LCDIF* lcd){
	lcd->PrintLine(lcd->m_CurStr, vpos, Stretch, start, end);
	lcd->ClearBuffer();
}

void wait::act(LCDIF* lcd){
	DelayMs(this->ms);
}

void symbol::act(LCDIF* lcd){
	char a[2] = {this->code,'\0'};
	lcd->strCat(lcd->m_CurStr, a, lcd->m_CharCount);
}
//sets number of digits after point for double
void precision::act(LCDIF* lcd){
	lcd->m_Precision = this->nod;
}
void clr(LCDIF* lcd){
	lcd->ClearScreen();
	lcd->ClearBuffer();
}
void update::act(LCDIF* lcd){
	lcd->m_LcdDevice.ssd1306_update(this->clear_buf);
	lcd->ClearBuffer();
}
void text_mode::act(LCDIF* lcd){
	lcd->m_TextMode = en;
	lcd->m_WordWrapping = wrap;
	lcd->m_SideOffset = hori_offset;
	lcd->m_TextAlignment = alignment;
}
char* LCDIF::getNextLine(char* dst, int& offset)
{
	int line_len = (ScreenSizeList[m_ScreenSize][0] - (m_SideOffset*2))/(FontSizeList[m_CurrentFontSize][0] + (m_Spacing - 1));
	int i = 0;
	int word_len = 0;
	while (dst[offset]!='\r' && dst[offset]!='\n' && dst[offset]!='\0')
	{
		if (i>=line_len)
		{
			if (word_len>=line_len)
			{
			}
			else if (word_len>0 && dst[offset]!=' ')
			{
				offset -= word_len;
				i -= word_len;
			}
			break;
		}
		word_len = (dst[offset]==' ') ? 0 : word_len+1;
		m_InnerStr[i] = dst[offset];
		offset++;
		i++;
	}
	m_InnerStr[i] = 0;
	while (dst[offset]=='\r' || /*dst[offset]=='\n' ||*/  dst[offset]==' ') offset++;
	if (dst[offset]=='\n') offset++;
	if (dst[offset] == 0) offset = -1;
	return m_InnerStr;
}
void line::act(LCDIF* lcd)
{
	lcd->PrintText(line_num, alignment);
	lcd->ClearBuffer();
}
void font::act(LCDIF* lcd)
{
	lcd->m_CurrentFontSize = f;
}

#ifdef __cplusplus
}
#endif
