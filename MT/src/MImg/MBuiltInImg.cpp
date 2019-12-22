#include "MBuiltInImg.h"
#include "bg.h"
#include "emt_hp.h"
#include <stdio.h>
#include <string.h>
extern "C" {
	img_bg imgBg;
	Img_emt_hp emtHp;

	static IMG_TEX builtInTex[] =
	{
		{ GET_NAME(img_bg), (ImgStruct*)&imgBg },
		{ GET_NAME(Img_emt_hp), (ImgStruct*)&emtHp },
		{ NULL, NULL },
	};


	ImgStruct * GetBuiltInImage(const char * img)
	{
		for (int i = 0; i < sizeof(builtInTex); ++i)
		{
			if (strcmp(img, builtInTex[i].name) == 0)
			{
				return builtInTex[i].img;
			}
		}
		return NULL;
	}
}