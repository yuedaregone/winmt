#include "MHttpRequest.h"
#include <WS2tcpip.h>
#include "MNetDef.h"

#define HTTP_HEAD_GET			"GET /%s HTTP/1.1\r\n"
#define HTTP_HEAD_POST			"POST /%s HTTP/1.1\r\n"
#define HTTP_HEAD_HOST			"Host:%s\r\n"
#define HTTP_HEAD_CONNECT		"Connection: keep-alive\r\n"
#define HTTP_HEAD_CACHE			"Cache-Control: max-age=0\r\n"
#define HTTP_HEAD_UPGRADE		"Upgrade-Insecure-Requests: 1\r\n"
#define HTTP_HEAD_USER			"User - Agent : Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.75 Safari/537.36\r\n"
#define HTTP_HEAD_ACCEPT		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\nAccept-Encoding: gzip, deflate\r\nAccept-Language: zh-CN,zh;q=0.9,en;q=0.8\r\n"
#define HTTP_HEAD_COOKIE		"Cookie: %s\r\n"
#define HTTP_LINE_END			"\r\n"

MHttpRequest::MHttpRequest(const char * url)
	: m_sendData(NULL)
{
	Reset();
	m_url = url;
	ParseURL();
}

MHttpRequest::~MHttpRequest()
{
	Reset();
}

std::string MHttpRequest::GetRequestPath()
{
	std::string defaultName = "index.html";
	if (m_reqContent.empty() || m_reqContent == "")
	{
		return defaultName;
	}
	size_t pos = 0;
	if ((pos = m_reqContent.find_first_of("?", 0)) != std::string::npos || (pos = m_reqContent.find_first_of("#", 0)) != std::string::npos)
	{
		return m_reqContent.substr(0, pos);
	}
	return m_reqContent;
}

std::string MHttpRequest::GetRequestFile()
{
	std::string p = GetRequestPath();
	size_t pos = p.find_last_of('/');
	if (pos != std::string::npos)
	{
		p = p.substr(pos + 1, p.length() - pos - 1);
	}
	if (p.empty() || p == "")
	{
		p = "index.html";
	}
	return p;
}

void MHttpRequest::Reset()
{
	m_head.clear();
	m_ips.clear();
	m_url.clear();
	m_host.clear();
	m_reqContent.clear();
	m_ssl = false;

	m_buffer.Reset();
	m_method = GET;

	if (m_sendData != NULL)
	{
		delete m_sendData;
		m_sendData = NULL;
	}		
}

int MHttpRequest::GetPort()
{
	return m_port;
}

void MHttpRequest::ParseURL()
{
	const char* http = "http://";
	const char* https = "https://";
	const char* url = m_url.c_str();

	std::string str;
	if (strncmp(url, http, strlen(http)) == 0)
	{
		m_ssl = false;
		m_port = 80;
		str = url + strlen(http);
	}		
	else if (strncmp(url, https, strlen(https)) == 0)
	{
		m_ssl = true;
		m_port = 443;
		str = url + strlen(https);
	}		
	else
	{
		m_ssl = false;
		str = url;
	}		

	size_t pos = str.find_first_of("/", 0);
	if (pos != std::string::npos)
	{
		m_host = str.substr(0, pos);
		m_reqContent = str.substr(pos + 1, str.length() - pos - 1);
	}
	else
	{
		m_host = str;
	}
}

void MHttpRequest::LoadIP()
{
	if (!m_ips.empty()) return;

	struct hostent* hname = gethostbyname(m_host.c_str());
	if (hname == NULL)
	{
		int errorret = GET_ERROR_CODE;
		LogError("Error:get host! %d\n", errorret);
		return;
	}
	char **pptr = NULL;
	char buff[16] = { 0 };

	switch (hname->h_addrtype)
	{
	case AF_INET:
	case AF_INET6:
		for (pptr = hname->h_addr_list; *pptr != NULL; ++pptr)
		{
			memset(buff, 0, sizeof(buff));
			const char* ip = inet_ntop(hname->h_addrtype, *pptr, buff, sizeof(buff));
			m_ips.push_back(ip);
		}
	default:
		break;
	}
}

uint32_t MHttpRequest::GetSendData(const uint8_t ** buff)
{
	LoadBuffer();
	*buff = m_buffer.GetBuffer();

	return m_buffer.GetSize();
}

const char * MHttpRequest::GetIP()
{
	LoadIP();
	if (m_ips.empty())
		return NULL;
	return m_ips[0].c_str();
}

std::vector<std::string> * MHttpRequest::GetIPS()
{
	LoadIP();	
	return &m_ips;
}

void MHttpRequest::LoadBuffer()
{
	if (!m_buffer.Empty()) return;

	const char* methodLine = NULL;

	if (m_method == GET)	
		m_buffer.WriteStringFormat(HTTP_HEAD_GET, m_reqContent.c_str());
	else	
		m_buffer.WriteStringFormat(HTTP_HEAD_POST, m_reqContent.c_str());
	
	m_buffer.WriteStringFormat(HTTP_HEAD_HOST, m_host.c_str());
	m_buffer.WriteString(m_head);
	m_buffer.WriteCStr(HTTP_HEAD_USER);
	m_buffer.WriteCStr(HTTP_HEAD_ACCEPT);
	m_buffer.WriteCStr(HTTP_LINE_END);

	if (m_sendData != NULL)
		m_buffer.WriteData(m_sendData);
}

void MHttpRequest::SetHead(const char * head)
{
	m_head.append(head);
	if (strncmp(head + strlen(head) - 2, HTTP_LINE_END, 2) != 0)
	{
		m_head.append(HTTP_LINE_END);
	}
}

void MHttpRequest::SetData(const uint8_t* data, uint32_t size)
{
	if (m_sendData == NULL)
		m_sendData = new MByteBuffer();
	m_sendData->WriteData(data, size);
}
