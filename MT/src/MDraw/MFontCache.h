#pragma once
#include <stdint.h>
#include "MSingleton.h"
#include "winfont.h"
#include <map>

class MFontCache : public MSingleton<MFontCache>
{
public:
	void Init();

	uint8_t * GetCharacterBuffer(int unicode);

	void Release();
private:
	uint8_t * GetCacheCharacter(int unicode);

	void GenCharacter(int unicode);
private:
	uint8_t * m_fontBuffer;
	std::map<int, int> m_fontRects;
	font_context m_fontContext;
};

#define g_mFontCache MFontCache::getInstance()