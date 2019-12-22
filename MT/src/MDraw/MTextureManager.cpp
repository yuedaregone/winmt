#include "MTextureManager.h"
#include "MTexture.h"
#include "MDefine.h"
#include "MBuiltInImg.h"


MTexture * MTextureManager::LoadTexture(const char* name, bool buildIn)
{
	if (m_textures.find(name) != m_textures.end())
	{
		return m_textures[name];
	}
	MTexture* tex = new MTexture();
	if (buildIn)
	{
		ImgStruct* img = GetBuiltInImage(name);
		if (img) tex->SetTexture(img);		 
	}
	else
	{
		tex->SetTexture(name);
	}	
	m_textures[name] = tex;
	return tex;
}

void MTextureManager::ReleaseUnusedTexture()
{
	std::map<std::string, MTexture*>::iterator it = m_textures.begin();
	for (; it != m_textures.end(); )
	{
		if (it->second->GetRefCount() <= 0)
		{
			delete it->second;
			m_textures.erase(it++);
		}
		else
		{
			++it;
		}
	}
}
