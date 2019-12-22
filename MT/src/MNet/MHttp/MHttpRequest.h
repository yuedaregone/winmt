#pragma once
#include <vector>
#include <string>
#include <stdint.h>
#include "MByteBuffer.h"

typedef enum
{
	GET = 0,
	POST = 1,

}MRequestMethod;

class MHttpClient;
class MHttpRequest
{
public:
	MHttpRequest(const char* url);
	~MHttpRequest();

	uint32_t GetSendData(const uint8_t** buff);

	void SetHead(const char* head);

	void SetData(const uint8_t* data, uint32_t size);

	void SetMethod(MRequestMethod method) { m_method = method; }

	std::string GetRequestPath();

	std::string GetRequestFile();

	void Reset();

	bool SSL() { return m_ssl; }

	int GetPort();

	void SetPort(int port) { m_port = port; }
private:
	void LoadIP();
	const char* GetIP();
	std::vector<std::string> * GetIPS();

	void LoadBuffer();

	void ParseURL();
private:
	std::string m_head;
	std::vector<std::string> m_ips;
	
	std::string m_url;
	std::string m_host;
	std::string m_reqContent;

	bool m_ssl;
	int m_port;

	MByteBuffer m_buffer;

	MByteBuffer* m_sendData;

	MRequestMethod m_method;

	friend MHttpClient;
};