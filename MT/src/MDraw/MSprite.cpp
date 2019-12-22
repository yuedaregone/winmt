#include "MSprite.h"
#include "MTexture.h"
#include "MTextureManager.h"
#include "pixelprocess.h"

MSprite::MSprite()
	: MDrawProtocol()
	, m_texture(NULL)	
{
}

MSprite * MSprite::CreateSprite(const char * fileName)
{
	MSprite* ret = new MSprite();
	MTexture* tex = g_mTextureMgr->LoadTexture(fileName);
	tex->Retain();
	ret->m_texture = tex;
	ret->m_rect = GetRect(0, 0, tex->GetWidth(), tex->GetHeight());
	return ret;
}

MSprite * MSprite::CreateBuiltinSprite(const char * fileName)
{
	MSprite* ret = new MSprite();
	MTexture* tex = g_mTextureMgr->LoadTexture(fileName, true);
	tex->Retain();
	ret->m_texture = tex;
	ret->m_rect = GetRect(0, 0, tex->GetWidth(), tex->GetHeight());
	return ret;
}

MSprite::~MSprite()
{
	m_texture->Release();
}

void MSprite::Draw()
{
	uint32_t* data = m_texture->GetData();
	uint32_t imgW = m_texture->GetWidth();
	uint32_t imgH = m_texture->GetHeight();

	PixelSetScale(m_drawState & DrawScale, m_scaleX, m_scaleY);
	PixelSetRotation(m_drawState & DrawRotation, m_rotation);
	PixelSetBlitFunc(eAlphaBlend);
	BlitContent(data, imgW, imgH, m_rect.left, m_rect.top);
}