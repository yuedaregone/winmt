#include <stdint.h>
#include "MTool.h"

extern "C" {

	int unicode_len(const char chr) {
		uint8_t c = (uint8_t)chr;
		if ((c & 0x80) == 0) {
			return 1;
		}
		else if ((c & 0xe0) == 0xc0) {
			return 2;
		}
		else if ((c & 0xf0) == 0xe0) {
			return 3;
		}
		else if ((c & 0xf8) == 0xf0) {
			return 4;
		}
		else if ((c & 0xfc) == 0xf8) {
			return 5;
		}
		else {
			return 6;
		}
	}

	int get_unicode(const char *str, int n) {
		int i;
		int unicode = str[0] & ((1 << (8 - n)) - 1);
		for (i = 1; i < n; i++) {
			unicode = unicode << 6 | ((uint8_t)str[i] & 0x3f);
		}
		return unicode;
	}	

	static inline int copystr(char *utf8, const char *str, int n) {
		int i;
		utf8[0] = str[0];
		int unicode = utf8[0] & ((1 << (8 - n)) - 1);
		for (i = 1; i < n; i++) {
			utf8[i] = str[i];
			unicode = unicode << 6 | (utf8[i] & 0x3f);
		}
		utf8[i] = 0;
		return unicode;
	}
}

std::string GetCurrentPath()
{
	char buff[512] = { 0 };
	GetCurrentDirectory(512, buff);
	return buff;
}

std::string GetFullPath(const char * str)
{
	std::string p = GetCurrentPath();
	p.append("\\");
	p.append(str);
	return p;
}
