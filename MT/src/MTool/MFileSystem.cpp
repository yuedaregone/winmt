#pragma once
#include <io.h>

extern "C" {
	bool IsFileExit(const char* filename) 
	{
		if (_access(filename, 0) == 0) {
			return true;
		}
		return false;
	}

}