#include "MHttpResponse.h"
#include "MNetDef.h"
#include <stdio.h>
#include "MByteBuffer.h"

MHttpResponse::MHttpResponse()	
{
	Clear();
}

static int FindIndex(const char* buff, uint32_t buffSize, const char* tag)
{
	uint32_t bufLen = buffSize;
	if (bufLen == 0) bufLen = strlen(buff);
	uint32_t tagLen = strlen(tag);
	for (uint32_t i = 0; i <= bufLen - tagLen; ++i)
	{
		if (strncmp((const char*)(buff + i), tag, tagLen) == 0)
		{
			return (int)i;
		}
	}
	return -1;
}

void MHttpResponse::OnResponseData(const uint8_t * data, uint32_t size)
{
	if (!m_headFinish)
	{
		const char* endTag = "\r\n\r\n";
		uint32_t endTagLen = strlen(endTag);

		int index = FindIndex((const char*)data, size, endTag);
		if (index == -1)
		{
			m_headBuffer.WriteData(data, size);
		}
		else
		{
			m_headBuffer.WriteData(data, index);
			m_headBuffer.WriteData((const uint8_t*)&"\0", 1);
			m_headFinish = true;
			ParseContentSize();
			m_contentBuffer.WriteData(data + index + endTagLen, size - index - endTagLen);
		}
	}
	else
	{
		m_contentBuffer.WriteData(data, size);
		if (m_contentBuffer.GetSize() >= m_contentSize)
		{
			m_contentFinish = true;
		}
	}
}

void MHttpResponse::ParseContentSize()
{
	if (!m_headFinish) return;

	const char* tag = "Content-Length:";
	size_t tagLen = strlen(tag);

	const char* ackHead = (const char*)m_headBuffer.GetBuffer();
	uint32_t headLength = m_headBuffer.GetSize();

	int index = FindIndex(ackHead, headLength, tag);
	if (index == -1)
	{
		m_contentSize = 0;
		return;
	}

	char contentBuf[32] = { 0 };
	uint32_t contentIdx = 0;
	uint32_t headIdx = index + tagLen;
	while (contentIdx < 32 && headIdx < headLength)
	{
		if (ackHead[headIdx] == ' ')
		{
			++headIdx;
			continue;
		}

		if (ackHead[headIdx] == '\r' && ackHead[headIdx + 1] == '\n')
			break;

		char c = ackHead[headIdx];
		if (c >= '0' && c <= '9')
		{
			contentBuf[contentIdx++] = c;			
		}
		++headIdx;
	}
	m_contentSize = atoi(contentBuf);
}

bool MHttpResponse::IsOK()
{
	return GetErrorCode() == 200;
}

uint32_t MHttpResponse::GetErrorCode()
{
	if (!m_headFinish) return 0;

	const char* headBuff = GetHead();
	int start = FindIndex(headBuff, 0, " ");
	int end = FindIndex(headBuff + start + 1, 0, " ");

	char buff[32] = { 0 };
	memcpy(buff, headBuff + start + 1, end);
	return atoi(buff);
}

const char * MHttpResponse::GetHead()
{
	return (const char*)m_headBuffer.GetBuffer();
}

const uint8_t * MHttpResponse::GetDataPtr()
{
	return m_contentBuffer.GetBuffer();
}

void MHttpResponse::Clear()
{
	m_headBuffer.Reset();
	m_headFinish = false;

	m_contentSize = 0;
	m_contentBuffer.Reset();
	m_contentFinish = false;
}
