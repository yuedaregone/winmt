#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string>

#ifndef BYTE_BUFFER_SIZE
#define BYTE_BUFFER_SIZE 8*1024
#endif

class MByteBuffer
{
public:
	MByteBuffer() 
		: m_buffer(NULL) 
		, m_capacity(0)
		, m_size(0)
	{
	}
	MByteBuffer(uint32_t capacity)
		: m_size(0)
	{
		InitBufferWithSize(capacity);
	}
	~MByteBuffer() 
	{
		if (m_buffer != NULL)
		{
			free(m_buffer);
		}
	}

	uint32_t GetSize() const { return m_size; }

	uint8_t* GetBuffer() const { return m_buffer; }

	void WriteData(const uint8_t* data, uint32_t size)
	{
		if (size == 0) return;
		CheckBufferSize(size);
		memcpy(m_buffer + m_size, data, size);
		m_size += size;
	}

	void WriteData(const MByteBuffer* buffer)
	{
		WriteData(buffer->GetBuffer(), buffer->GetSize());
	}

	void WriteStringFormat(const char* s, ...)
	{
		char buff[BYTE_BUFFER_SIZE] = { 0 };
		va_list va;
		va_start(va, s);
		vsnprintf(buff, BYTE_BUFFER_SIZE, s, va);
		va_end(va);

		WriteData((const uint8_t*)buff, (uint32_t)strlen(buff));
	}

	void WriteString(const std::string& s)
	{
		WriteData((const uint8_t*)s.c_str(), (uint32_t)s.length());
	}

	void WriteCStr(const char* s)
	{
		WriteData((const uint8_t*)s, (uint32_t)strlen(s));
	}

	void Reset()
	{
		m_size = 0;
	}

	bool Empty() { return m_size == 0; }
private:
	void CheckBufferSize(uint32_t needSize)
	{
		if (m_buffer == NULL)
		{
			m_buffer = (uint8_t*)malloc(BYTE_BUFFER_SIZE);
			m_capacity = BYTE_BUFFER_SIZE;
		}
		if (m_capacity - m_size < needSize)
		{
			uint32_t cap = m_capacity;
			if (needSize > m_capacity)			
				cap = RecalcCapacity(needSize);			
			cap *= 2;

			uint8_t* buff = (uint8_t*)malloc(cap);
			memcpy(buff, m_buffer, m_size);
			m_capacity = cap;
			m_buffer = buff;
		}
	}

	uint32_t RecalcCapacity(uint32_t needSize)
	{				
		uint32_t cap = needSize;
		if (needSize % 1024 != 0)
		{
			cap = (needSize / 1024 + 1) * 1024;
		}		
		return cap;
	}

	void InitBufferWithSize(uint32_t cap)
	{
		m_capacity = RecalcCapacity(cap);
		m_buffer = (uint8_t*)malloc(m_capacity);
	}
private:
	uint8_t* m_buffer;
	uint32_t m_capacity;
	uint32_t m_size;
};
