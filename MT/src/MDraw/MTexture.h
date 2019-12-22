#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include "MDefine.h"
#include "MTool.h"

class MTextureManager;
class MTexture
{
public:
	MTexture() : m_data(NULL), m_height(0), m_width(0), m_hasAlpha(true), m_heapData(false) {}

	~MTexture()
	{		
		if (m_heapData && m_data != NULL)
		{
			delete[] m_data;
			m_data = NULL;
		}
	}	

	uint32_t GetWidth() { return m_width; }
	uint32_t GetHeight() { return m_height; }

	uint32_t* GetData() { return m_data; }
	bool HasAlpha() { return m_hasAlpha; }

	void Retain() { ++m_refCount; }
	void Release() { --m_refCount; }


private:
	void SetTexture(const char* fileName)
	{
		std::string path = GetFullPath(fileName);

		FILE* fp = fopen(path.c_str(), "rb+");
		if (fp == NULL)
		{
			printf("Error: Can't open file %s", fileName);
			return;
		}
		int alpha = fgetc(fp);
		uint32_t size[2];
		fread((void*)size, sizeof(uint32_t), 1, fp);
		fread((void*)(size + 1), sizeof(uint32_t), 1, fp);

		uint32_t* data = new uint32_t[size[0] * size[1]];
		if (alpha)
		{			
			fread((void*)data, size[0] * size[1] * sizeof(uint32_t), 1, fp);
		}
		else
		{
			uint32_t len = size[0] * size[1] * (sizeof(uint32_t) - 1);
			uint8_t* temp = new uint8_t[len];
			fread((void*)temp, len, 1, fp);			
			
			for (int i = 0; i < (int)(size[0] * size[1]); ++i)
			{
				memcpy(data + i * sizeof(uint32_t), temp + i * (sizeof(uint32_t)-1), sizeof(uint32_t) - 1);
			}	
			delete[] temp;
		}
		fclose(fp);
		m_heapData = true;
		SetTexture(data, size[0], size[1], (bool)alpha);
	}

	void SetTexture(ImgStruct* img)
	{
		m_heapData = false;
		SetTexture(img->data, img->width, img->height);
	}
	
	int32_t GetRefCount() { return m_refCount; }
private:
	void SetTexture(uint32_t* data, uint32_t w, uint32_t h, bool hasAlpha = true)
	{
		m_data = data;
		m_width = w;
		m_height = h;
		m_hasAlpha = hasAlpha;
	}

private:
	bool m_hasAlpha;
	bool m_heapData;
	int32_t m_refCount;
	uint32_t m_width;
	uint32_t m_height;
	uint32_t* m_data;

	friend MTextureManager;
};
