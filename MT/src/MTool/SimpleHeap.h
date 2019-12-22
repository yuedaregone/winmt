#pragma once
#include <stdint.h>
#define BLOCK_SIZE (32 * 1024)

class SimpleHeap
{
private:
	uint8_t *mBlock;
	uint8_t *mFreeMarker;
	uint32_t mSpaceAvailable;
	static uint32_t sBlockCount;
	static SimpleHeap *sFirst, *sLast;
	static uint8_t *sMostRecentlyAllocated;
	SimpleHeap *mNextBlock;

	static SimpleHeap *CreateBlock();
	SimpleHeap(); 
	~SimpleHeap();
public:
	static uint8_t* Malloc(const char * aBuf, uint32_t aLength = -1);
	static void* Malloc(uint32_t aSize);
	static void Delete(void *aPtr);
};