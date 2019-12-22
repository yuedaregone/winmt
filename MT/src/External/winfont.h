//////////////////////////////////////////////////////////////////////////
/*
 * ejoy's font for window
*/
//////////////////////////////////////////////////////////////////////////
#pragma once

#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "MDefine.h"
#include "MMath.h"

extern "C"
{
	struct font_context {
		int w;
		int h;
		int ascent;
		void * font;
		void * dc;
	};

	inline void font_create(int font_size, struct font_context *ctx) {
		TEXTMETRIC tm;
		HFONT f = CreateFontW(
			font_size, 0,
			0, 0,
			FW_NORMAL,
			FALSE, FALSE, FALSE,
			CHINESEBIG5_CHARSET,
			OUT_CHARACTER_PRECIS,
			CLIP_CHARACTER_PRECIS,
			DEFAULT_QUALITY,
			FF_MODERN,
			NULL
		);

		HDC dc = CreateCompatibleDC(NULL);
		SelectObject(dc, f);
		ctx->font = (void *)f;
		ctx->dc = (void *)dc;
		GetTextMetrics(dc, &tm);
		ctx->h = tm.tmHeight + 1;
		ctx->ascent = tm.tmAscent;
	}

	inline void font_release(struct font_context *ctx) {
		DeleteObject((HFONT)ctx->font);
		DeleteDC((HDC)ctx->dc);
	}

	static MAT2 mat2 = { {0,1},{0,0},{0,0},{0,1} };

	inline void font_size(int unicode, struct font_context *ctx) {
		GLYPHMETRICS gm;

		GetGlyphOutlineW(
			(HDC)ctx->dc,
			unicode,
			GGO_GRAY8_BITMAP,
			&gm,
			0,
			NULL,
			&mat2
		);

		ctx->w = gm.gmCellIncX + 1;
	}

	inline void font_glyph(int unicode, void * buffer, struct font_context *ctx) {
		GLYPHMETRICS gm;
		memset(&gm, 0, sizeof(gm));

		ARRAY(uint8_t, tmp, ctx->w * ctx->h);
		memset(tmp, 0, ctx->w * ctx->h);

		GetGlyphOutlineW(
			(HDC)ctx->dc,
			unicode,
			GGO_GRAY8_BITMAP,
			&gm,
			ctx->w * ctx->h,
			tmp,
			&mat2
		);

		int w = (gm.gmBlackBoxX + 3) & ~3;
		int h = gm.gmBlackBoxY;

		uint8_t * buf = (uint8_t *)buffer;
		int offx = gm.gmptGlyphOrigin.x;
		int offy = ctx->ascent - gm.gmptGlyphOrigin.y;
		assert(offx >= 0);
		assert(offy >= 0);
		assert(offx + (int)gm.gmBlackBoxX <= ctx->w);
		assert(offy + h <= ctx->h);

		int i, j;

		for (i = 0; i < h; i++) {
			for (j = 0; j < (int)gm.gmBlackBoxX; j++) {
				int src = tmp[i*w + j];
				buf[(i + offy)*ctx->w + j + offx] = src *255 / 64;
			}
		}
	}

	static void	gen_outline(int w, int h, uint8_t *buffer, uint8_t *dest) {
		int i, j;
		for (i = 0; i < h; i++) {
			uint8_t * output = dest + i*w;
			uint8_t * line = buffer + i*w;
			uint8_t * prev;
			uint8_t * next;
			if (i == 0) {
				prev = line;
			}
			else {
				prev = line - w;
			}
			if (i == h - 1) {
				next = line;
			}
			else {
				next = line + w;
			}
			for (j = 0; j < w; j++) {
				int left, right;
				if (j == 0) {
					left = 0;
				}
				else {
					left = 1;
				}
				if (j == w - 1) {
					right = 0;
				}
				else {
					right = 1;
				}
				int n1 = MAX_4(line[j - left], line[j + right], prev[j], next[j]);
				int n2 = MAX_4(prev[j - left], prev[j + right], next[j - left], next[j + right]);
				int edge = (n1 * 3 + n2) / 4;
				if (line[j] == 0) {
					output[j] = edge / 2;
				}
				else {
					output[j] = line[j] / 2 + 128;
				}
			}
			if (output[0] > 128) {
				output[0] /= 2;
			}
			if (output[w - 1] > 128) {
				output[w - 1] /= 2;
			}
		}
	}
}

