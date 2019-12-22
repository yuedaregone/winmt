#pragma once
#include "MDefine.h"
extern "C" {
	struct IMG_TEX
	{
		const char* name;
		ImgStruct* img;
	};

	ImgStruct* GetBuiltInImage(const char* img);	
}
