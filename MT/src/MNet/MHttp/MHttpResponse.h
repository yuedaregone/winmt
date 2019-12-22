#pragma once
#include <stdint.h>
#include "MByteBuffer.h"

class MHttpClient;
class MHttpResponse
{
public:
	const char* GetHead();

	const uint8_t* GetDataPtr();

	uint32_t GetContentSize() { return m_contentSize; }

	MByteBuffer* GetContentBuffer() { return &m_contentBuffer; }

	bool Finished() { return m_headFinish && m_contentFinish; }

	bool IsOK();

	uint32_t GetErrorCode();

	void Clear();
private:
	MHttpResponse();
	void OnResponseData(const uint8_t* data, uint32_t size);
	void ParseContentSize();
private:
	MByteBuffer m_headBuffer;
	bool m_headFinish;	

	uint32_t m_contentSize;
	MByteBuffer m_contentBuffer;
	bool m_contentFinish;

	friend MHttpClient;
};
