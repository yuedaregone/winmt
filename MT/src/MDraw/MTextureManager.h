#pragma once
#include <map>
#include "MSingleton.h"

class MTexture;
class MTextureManager : public MSingleton<MTextureManager>
{
public:
	MTexture* LoadTexture(const char* name, bool buildIn = false);

	void ReleaseUnusedTexture();
private:
	std::map<std::string, MTexture*> m_textures;
};

#define g_mTextureMgr MTextureManager::getInstance()
