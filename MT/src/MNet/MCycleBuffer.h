#pragma once
#include <stdint.h>
#include "MNetDef.h"

#define BUFFER_CYCLE_SIZE 0x00040000
class MCycleBuffer
{
public:
	MCycleBuffer()
	{
		Reset();
	}

	void Reset()
	{
		m_readIdx = 0;
		m_writeIdx = 0;
		m_dataSize = 0;
	}

	uint32_t Space()
	{
		return BUFFER_CYCLE_SIZE - m_dataSize;
	}

	uint32_t Size()
	{
		return m_dataSize;
	}

	uint32_t Read(uint8_t* buffer, uint32_t len)
	{
		uint32_t dataSize = m_dataSize < len ? m_dataSize : len;
		if (dataSize <= 0)
		{
			return 0;
		}
		if (m_readIdx < m_writeIdx)
		{
			memcpy(buffer, m_buffer + m_readIdx, dataSize);
			m_readIdx += dataSize;
		}
		else
		{
			uint32_t r = BUFFER_CYCLE_SIZE - m_readIdx;
			if (dataSize <= r)
			{
				memcpy(buffer, m_buffer + m_readIdx, dataSize);
				if (dataSize == r) m_readIdx = 0;
				else m_readIdx += dataSize;
			}
			else
			{
				memcpy(buffer, m_buffer + m_readIdx, r);
				memcpy(buffer + r, m_buffer, dataSize - r);
				m_readIdx = dataSize - r;
			}
		}
		m_dataSize -= dataSize;
		return dataSize;
	}

	uint32_t Write(uint8_t* buffer, uint32_t len)
	{
		uint32_t emptySize = BUFFER_CYCLE_SIZE - m_dataSize;
		uint32_t dataSize = emptySize < len ? emptySize : len;
		if (dataSize <= 0)
		{
			return 0;
		}
		if (m_writeIdx >= m_readIdx)
		{
			uint32_t r = BUFFER_CYCLE_SIZE - m_writeIdx;
			if (dataSize <= r)
			{
				memcpy(m_buffer + m_writeIdx, buffer, dataSize);
				if (dataSize == r) m_writeIdx = 0;
				else m_writeIdx += dataSize;
			}
			else
			{
				memcpy(m_buffer + m_writeIdx, buffer, r);
				memcpy(m_buffer, buffer + r, dataSize - r);
				m_writeIdx = dataSize - r;
			}
		}
		else
		{
			memcpy(m_buffer + m_writeIdx, buffer, dataSize);
			m_writeIdx += dataSize;
		}
		m_dataSize += dataSize;
		return dataSize;
	}

	uint32_t PeekRead(uint8_t* buffer, uint32_t len)
	{
		uint32_t dataSize = m_dataSize < len ? m_dataSize : len;
		if (dataSize <= 0)
		{
			return 0;
		}
		if (m_readIdx < m_writeIdx)
		{
			memcpy(buffer, m_buffer + m_readIdx, dataSize);
			//m_readIdx += dataSize;
		}
		else
		{
			uint32_t r = BUFFER_CYCLE_SIZE - m_readIdx;
			if (dataSize <= r)
			{
				memcpy(buffer, m_buffer + m_readIdx, dataSize);
				//if (dataSize == r) m_readIdx = 0;
				//else m_readIdx += dataSize;
			}
			else
			{
				memcpy(buffer, m_buffer + m_readIdx, r);
				memcpy(buffer + r, m_buffer, dataSize - r);
				//m_readIdx = dataSize - r;
			}
		}
		//m_dataSize -= dataSize;
		return dataSize;
	}

	void PeekSetRead(uint32_t len)
	{
		if (len == 0)
		{
			return;
		}
		uint32_t dataSize = m_dataSize < len ? m_dataSize : len;
		if (dataSize <= 0)
		{
			return;
		}
		if (m_readIdx < m_writeIdx)
		{
			m_readIdx += dataSize;
		}
		else
		{
			uint32_t r = BUFFER_CYCLE_SIZE - m_readIdx;
			if (dataSize <= r)
			{
				if (dataSize == r) m_readIdx = 0;
				else m_readIdx += dataSize;
			}
			else
			{
				m_readIdx = dataSize - r;
			}
		}
		m_dataSize -= dataSize;
	}
private:
	uint32_t m_readIdx = 0;
	uint32_t m_writeIdx = 0;
	uint32_t m_dataSize = 0;
	uint8_t m_buffer[BUFFER_CYCLE_SIZE];
};
