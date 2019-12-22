#pragma once
#include <math.h>
//#pragma intrinsic(cos) 
//#pragma intrinsic(sin) 

#define MAX_2(a, b) ((a)>(b)?(a):(b))
#define MAX_4(a, b, c, d) (MAX_2(MAX_2(a, b), MAX_2(c, d)))

#define MIN_2(a, b) ((a)>(b)?(b):(a))
#define MIN_4(a, b, c, d) (MIN_2(MIN_2(a, b), MIN_2(c, d)))

#define Sin(a) sin(a)
#define Cos(a) cos(a)

#define Clamp(x, a, b) ((x)<(a)?(a):((x)>(b)?(b):(x)))
#define Clamp01(x) Clamp(x, 0.0f, 1.0f)
