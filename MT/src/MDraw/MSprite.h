#pragma once
#include <stdint.h>
#include "MDefine.h"
#include "MTool.h"
#include "MDrawProtocol.h"

class MTexture;
class MSprite : public MDrawProtocol
{
public:
	static MSprite* CreateSprite(const char* fileName);
	static MSprite* CreateBuiltinSprite(const char* fileName);
	virtual ~MSprite();
public:
	virtual void Draw();
private:
	MSprite();

private:		
	MTexture* m_texture;	
};