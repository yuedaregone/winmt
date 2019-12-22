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
	std::vector<CharData*> m_chData;
};
