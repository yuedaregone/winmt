#include "MLabel.h"
#include "winfont.h"
#include "MTool.h"
#include "pixelprocess.h"

MLable * MLable::CreateLabel(const char * content, uint32_t fontSize)
{
	MLable* lb = new MLable();
	lb->m_fontSize = fontSize;
	lb->SetContent(content);
	return lb;
}

void MLable::SetContent(const char * content)
{
	m_str = content;

	font_context ctx;
	font_create(m_fontSize, &ctx);

	int n = strlen(content);

	int i = 0;
	for ( ; i < n; )
	{
		int len = unicode_len(content[i]);
		int unicode = get_unicode(content + i, len);
		CharData * dt = GenCharacterData(unicode, &ctx);
		m_chData.push_back(dt);
		i += len;
	}

	font_release(&ctx);

	uint32_t w = 0;
	uint32_t h = 0;
	std::vector<CharData*>::iterator it = m_chData.begin();
	for (int i = 0; it != m_chData.end(); ++it, ++i)
	{
		w += m_chData[i]->w;
		h = m_chData[i]->h > h ? m_chData[i]->h : h;
	}
	m_rect.right = m_rect.left + w;
	m_rect.bottom = m_rect.top + h;
}

MLable::~MLable()
{
	std::vector<CharData*>::iterator it = m_chData.begin();
	for (; it != m_chData.end(); ++it)
	{
		ReleaseCharData(*it);
	}
	m_chData.clear();
}

MLable::MLable()
	: m_fontSize(16)
{
}

CharData * MLable::GenCharacterData(int unicode, font_context* ctx)
{	
	font_size(unicode, ctx);

	CharData * dt = (CharData *)malloc(sizeof(CharData));
	dt->w = ctx->w;
	dt->h = ctx->h;
	dt->buffer = (uint8_t*)malloc(ctx->w * ctx->h);

	uint8_t* buf = (uint8_t*)_malloca(ctx->w * ctx->h);
	memset(buf, 0, ctx->w * ctx->h);
	font_glyph(unicode, buf, ctx);

	gen_outline(ctx->w, ctx->h, buf, dt->buffer);

	return dt;
}

void MLable::ReleaseCharData(CharData * ch)
{
	free(ch->buffer);
	free(ch);
}

void MLable::Draw()
{
	std::vector<CharData*>::iterator it = m_chData.begin();
	for (int i = 0; it != m_chData.end(); ++it, ++i)
	{
		BlitGrayToColor((*it)->buffer, (*it)->w, (*it)->h, m_rect.left + i * (*it)->w, m_rect.top, m_color);
	}
}