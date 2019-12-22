#pragma once
#include <malloc.h>

#define GET_NAME(t) #t

#define ARRAY(type, name, size) type* name = (type*)_alloca((size) * sizeof(type))

struct ImgStruct
{
	int width = 0;
	int height = 0;
	unsigned int data[1];
};
