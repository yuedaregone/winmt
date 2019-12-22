#include <malloc.h>
#include "MFontCache.h"

#define LB_TEX_W 1024
#define LB_TEX_H 1024
#define FONT_SIZE 16

#define V_NUM (LB_TEX_W / FONT_SIZE)
#define H_NUM (LB_TEX_H / FONT_SIZE)

void MFontCache::Init()
{
	m_fontBuffer = (uint8_t*)_malloca(LB_TEX_W * LB_TEX_H);
	font_create(FONT_SIZE, &m_fontContext);
}

uint8_t * MFontCache::GetCharacterBuffer(int unicode)
{
	uint8_t * ret = GetCacheCharacter(unicode);
	if (!ret) return ret;

	font_size(unicode, &m_fontContext);

	uint8_t * buffer = (uint8_t*)_malloca(FONT_SIZE * FONT_SIZE);
	//font_glyph(unicode, , &m_fontContext);
	return NULL;
}

void MFontCache::Release()
{
	font_release(&m_fontContext);
}

uint8_t * MFontCache::GetCacheCharacter(int unicode)
{
	std::map<int, int>::iterator it = m_fontRects.find(unicode);
	if (it != m_fontRects.end())
	{
		int index = it->second;
		int idy = index % V_NUM;
		int idx = index - idy * H_NUM;

		return m_fontBuffer + idy * FONT_SIZE * LB_TEX_W + idx * FONT_SIZE;
	}
	return NULL;
}

void MFontCache::GenCharacter(int unicode)
{
	
}
