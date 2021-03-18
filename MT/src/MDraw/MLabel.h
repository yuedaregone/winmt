#pragma once
#include <string>
#include <stdint.h>
#include <vector>
#include "MSprite.h"

struct CharData 
{
	uint16_t w;
	uint16_t h;
	uint8_t* buffer;
};

struct font_context;

class MLable : public MDrawProtocol
{
public:
	static MLable* CreateLabel(const char* content, uint32_t fontSize);
	
	void SetContent(const char* content);

	inline void SetColor(uint32_t color) { m_color = color; }

	inline void SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) 
	{
		m_color = ConvertColor(r, g, b, a); 
	}

	~MLable();
public:
	virtual void Draw();	
private:
	MLable();

	CharData* GenCharacterData(int unicode, font_context* ctx);

	void ReleaseCharData(CharData* ch);
private:
	std::string m_str;
	uint32_t m_fontSize;
	uint32_t m_color;
	std::vector<CharData*> m_chData;
};
