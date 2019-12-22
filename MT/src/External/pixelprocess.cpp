#include "pixelprocess.h"
#include "string.h"

extern "C" {
	typedef void (*PixelFunc)(uint8_t* src, uint8_t* dst);	
	static const float PixelRate = 1.0f / 255.0f;

	struct BlitContext
	{
		uint32_t* dst;
		uint32_t W;
		uint32_t H;
		PixelFunc func;
		float scaleX;
		float scaleY;
		bool scale;
		float rotation;		
		bool rot;
	};

	static struct BlitContext ctx;	

	static inline void PixelOverride(uint8_t* src, uint8_t* dst)
	{
		*(dst + 0) = *(src + 1);
		*(dst + 1) = *(src + 2);
		*(dst + 2) = *(src + 3);
		*(dst + 3) = *(src + 0);
	}

	static inline void PixelAlphaBlend(uint8_t* src, uint8_t* dst)
	{
		int a = *(src + 0);
		int b = *(src + 1);
		int g = *(src + 2);
		int r = *(src + 3);

		int db = *dst;
		int dg = *(dst + 1);
		int dr = *(dst + 2);
		int da = *(dst + 3);

		*(dst + 0) = db - int((db - b) * a * PixelRate);
		*(dst + 1) = dg - int((dg - g) * a * PixelRate);
		*(dst + 2) = dr - int((dr - r) * a * PixelRate);
		*(dst + 3) = da - int((da - a) * a * PixelRate);
	}

	void PixelSetContext(uint32_t* buf, uint32_t wWidth, uint32_t wHeight)
	{
		memset(&ctx, 0, sizeof(ctx));

		ctx.dst = buf;
		ctx.W = wWidth;
		ctx.H = wHeight;		
		ctx.func = PixelAlphaBlend;		
	}

	void PixelSetBlitFunc(PixelType type)
	{
		switch (type)
		{
		case eOverride:
			ctx.func = PixelOverride;
		case eAlphaBlend:
			ctx.func = PixelAlphaBlend;		
		default:
			break;
		}
	}

	void PixelSetScale(bool enabal, float scaleX, float scaleY)
	{
		ctx.scaleX = scaleX;
		ctx.scaleY = scaleY;
		ctx.scale = enabal;
	}

	void PixelSetRotation(bool enabal, float rot)
	{
		ctx.rotation = rot;
		ctx.rot = enabal;
	}	

	static inline void BlitRGBA(uint32_t * srcData, uint32_t srcWidth, uint32_t srcHeight, uint32_t x, uint32_t y)
	{
		uint32_t w = x + srcWidth > ctx.W ? ctx.W - x : srcWidth;
		uint32_t h = y + srcHeight > ctx.H ? ctx.H - y : srcHeight;

		for (uint32_t j = 0; j < h; ++j)
		{
			for (uint32_t i = 0; i < w; ++i)
			{
				uint8_t* pixel = (uint8_t*)(srcData + j * srcWidth + i);
				uint8_t* dest = (uint8_t*)(ctx.dst + (y + j) * ctx.W + x + i);

				(*ctx.func)(pixel, dest);
			}
		}
	}	

	static inline void BlitRGBAScale(uint32_t * srcData, uint32_t srcWidth, uint32_t srcHeight, uint32_t x, uint32_t y)
	{
		uint32_t dstWidth = uint32_t(ctx.scaleX * (float)srcWidth);
		uint32_t dstHeight = uint32_t(ctx.scaleY * (float)srcHeight);

		uint32_t maxX = MIN_2(x + dstWidth, ctx.W);
		uint32_t maxY = MIN_2(y + dstHeight, ctx.H);

		float ratioX = (float)srcWidth / (float)dstWidth;
		float ratioY = (float)srcHeight / (float)dstHeight;

		for (uint32_t j = y; j < maxY; ++j)
		{
			for (uint32_t i = x; i < maxX; ++i)
			{
				uint32_t ox = (uint32_t)(ratioX * (i - x));
				uint32_t oy = (uint32_t)(ratioY * (j - y));

				if (ox < 0 || ox >= srcWidth || oy < 0 || oy >= srcHeight)
					continue;

				uint8_t* pixel = (uint8_t*)(srcData + oy * srcWidth + ox);		
				uint8_t* dest = (uint8_t*)(ctx.dst + j * ctx.W + i);

				(*ctx.func)(pixel, dest);
			}
		}
	}

	static inline void BlitRGBAScaleRotation(uint32_t * srcData, uint32_t srcWidth, uint32_t srcHeight, uint32_t x, uint32_t y)
	{
		uint32_t dstWidth = uint32_t(ctx.scaleX * (float)srcWidth);
		uint32_t dstHeight = uint32_t(ctx.scaleY * (float)srcHeight);

		float wx = (float)x / (float)ctx.W;
		float wy = (float)y / (float)ctx.H;

		float wx2 = (float)(x + srcWidth) / (float)ctx.W;
		float wy2 = (float)(y + srcHeight) / (float)ctx.H;

		float cx = (wx + wx2) * 0.5f;
		float cy = (wy + wy2) * 0.5f;

		float c = Cos(ctx.rotation); float s = Sin(ctx.rotation);

		float x1 = (wx - cx) * c + (wy - cy) * s + cx;
		float y1 = -(wx - cx) * s + (wy - cy) * c + cy;

		float x2 = (wx2 - cx) * c + (wy2 - cy) * s + cx;
		float y2 = -(wx2 - cx) * s + (wy2 - cy) * c + cy;

		float x3 = (wx2 - cx) * c + (wy - cy) * s + cx;
		float y3 = -(wx2 - cx) * s + (wy - cy) * c + cy;

		float x4 = (wx - cx) * c + (wy2 - cy) * s + cx;
		float y4 = -(wx - cx) * s + (wy2 - cy) * c + cy;

		float minfX = MIN_4(x1, x2, x3, x4); minfX = Clamp01(minfX);
		float maxfX = MAX_4(x1, x2, x3, x4); maxfX = Clamp01(maxfX);
		float minfY = MIN_4(y1, y2, y3, y4); minfY = Clamp01(minfY);
		float maxfY = MAX_4(y1, y2, y3, y4); maxfY = Clamp01(maxfY);

		uint32_t minX = (uint32_t)(minfX * ctx.W);
		uint32_t maxX = (uint32_t)(maxfX * ctx.W);
		uint32_t minY = (uint32_t)(minfY * ctx.H);
		uint32_t maxY = (uint32_t)(maxfY * ctx.H);

		for (uint32_t j = minY; j < maxY; ++j)
		{
			for (uint32_t i = minX; i < maxX; ++i)
			{
				float ux = (float)i / (float)ctx.W;
				float uy = (float)j / (float)ctx.H;

				float rx = c * (ux - cx) - s * (uy - cy) + cx;
				float ry = s * (ux - cx) + c * (uy - cy) + cy;

				uint32_t ox = (uint32_t)(rx * (float)ctx.W);
				uint32_t oy = (uint32_t)(ry * (float)ctx.H);

				if (ox < x || ox >= x + srcWidth || oy < y || oy >= y + srcHeight)
					continue;

				uint8_t* pixel = (uint8_t*)(srcData + (oy - y) * srcWidth + ox - x);
				uint8_t* dest = (uint8_t*)(ctx.dst + j * ctx.W + i);

				(*ctx.func)(pixel, dest);
			}
		}
	}

	
	void BlitContent(uint32_t * srcData, uint32_t srcWidth, uint32_t srcHeight, uint32_t x, uint32_t y)
	{
		if (ctx.rot)
		{
			if (!ctx.scale)
			{
				ctx.scaleX = 1.0f;
				ctx.scaleY = 1.0f;
			}
			BlitRGBAScaleRotation(srcData, srcWidth, srcHeight, x, y);
			return;
		}
		if (ctx.scale)
		{
			BlitRGBAScale(srcData, srcWidth, srcHeight, x, y);
			return;
		}
		BlitRGBA(srcData, srcWidth, srcHeight, x, y);
	}

	void BlitGrayToColor(uint8_t* srcData, uint32_t srcWidth, uint32_t srcHeight, uint32_t x, uint32_t y, uint32_t colorRGBA)
	{
		if (x >= ctx.W || y >= ctx.H) return;

		uint32_t w = x + srcWidth > ctx.W ? ctx.W - x : srcWidth;
		uint32_t h = y + srcHeight > ctx.H ? ctx.H - y : srcHeight;

		float red = (colorRGBA >> 24 & 0xFF) / 255.0f;
		float green = (colorRGBA >> 16 & 0xFF) / 255.0f;
		float blue = (colorRGBA >> 8 & 0xFF) / 255.0f;
		float alpha = (colorRGBA & 0xFF) / 255.0f;

		for (uint32_t j = 0; j < h; ++j)
		{
			for (uint32_t i = 0; i < w; ++i)
			{
				uint8_t gray = *(srcData + j * srcWidth + i);
				if (gray != 0)
				{
					uint8_t* dest = (uint8_t*)(ctx.dst + (y + j) * ctx.W + x + i);

					*(dest + 0) = (uint8_t)(blue * gray);
					*(dest + 1) = (uint8_t)(green * gray);
					*(dest + 2) = (uint8_t)(red * gray);
					*(dest + 3) = (uint8_t)(alpha * gray);
				}
			}
		}
	}
}
