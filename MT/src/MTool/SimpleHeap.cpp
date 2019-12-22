#include "SimpleHeap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SimpleHeap *SimpleHeap::sFirst = NULL;
SimpleHeap *SimpleHeap::sLast = NULL;
uint8_t *SimpleHeap::sMostRecentlyAllocated = NULL;
uint32_t SimpleHeap::sBlockCount = 0;

SimpleHeap * SimpleHeap::CreateBlock()
{
	SimpleHeap* block;
	if (!(block = new SimpleHeap()))
		return NULL;
	if (!(block->mBlock = block->mFreeMarker = (uint8_t *)malloc(BLOCK_SIZE)))
	{
		delete block;
		return NULL;
	}
	block->mSpaceAvailable = BLOCK_SIZE;
	sLast = block;
	++sBlockCount;
	return block;
}

SimpleHeap::SimpleHeap()
	: mNextBlock(NULL)
{
}

SimpleHeap::~SimpleHeap()
{
	if (mBlock)
		free(mBlock);
	return;
}

uint8_t * SimpleHeap::Malloc(const char * aBuf, uint32_t aLength)
{
	if (!aBuf || !*aBuf)
		return NULL;
	if (aLength == -1)
		aLength = strlen(aBuf);
	uint8_t* nBuf;
	if (!(nBuf = (uint8_t*)Malloc(aLength + 1)))
	{
		return NULL;
	}
	if (aBuf)
		memcpy(nBuf, aBuf, aLength);
	nBuf[aLength] = '\0';
	return nBuf;
}

void * SimpleHeap::Malloc(uint32_t aSize)
{
	if (aSize < 1 || aSize > BLOCK_SIZE)
		return NULL;
	if (!sFirst)
		if (!(sFirst = CreateBlock()))
			return NULL;
	if (aSize > sLast->mSpaceAvailable)
		if (!(sLast->mNextBlock = CreateBlock()))
			return NULL;
	sMostRecentlyAllocated = sLast->mFreeMarker;

	uint32_t remainder = aSize % sizeof(void *);
	uint32_t sizeConsumed = remainder ? aSize + (sizeof(void*) - remainder) : aSize;
	sLast->mFreeMarker += sizeConsumed;
	sLast->mSpaceAvailable -= sizeConsumed;
	return (void*)sMostRecentlyAllocated;
}

void SimpleHeap::Delete(void * aPtr)
{
	if (aPtr != sMostRecentlyAllocated || !sMostRecentlyAllocated)
		return;
	uint32_t sMostRecentlyAllocatedSize = sLast->mFreeMarker - sMostRecentlyAllocated;
	sLast->mFreeMarker -= sMostRecentlyAllocatedSize;
	sLast->mSpaceAvailable += sMostRecentlyAllocatedSize;
	sMostRecentlyAllocated = NULL;
}
