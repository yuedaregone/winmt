#include "MNetDef.h"
#include <winhttp.h>
#include <Windows.h>
#include <string>
#include <sstream>

#include "MHttpTools.h"
#pragma comment(lib, "winhttp.lib")

namespace MHttpTools {

	static std::wstring widen(const std::string& str)
	{
		std::wostringstream wstm;
		const std::ctype<wchar_t>& ctfacet = std::use_facet< std::ctype<wchar_t> >(wstm.getloc());
		for (size_t i = 0; i < str.size(); ++i)
			wstm << ctfacet.widen(str[i]);
		return wstm.str();
	}

	std::string RequestHttpGet(const char* url) 
	{
		std::string result;
		std::wstring wurl = widen(url);

		URL_COMPONENTS urlComponent;
		memset(&urlComponent, 0, sizeof(urlComponent));
		urlComponent.dwStructSize = sizeof(urlComponent);
		WCHAR wchScheme[64] = { 0 };
		urlComponent.lpszScheme = wchScheme;
		urlComponent.dwSchemeLength = ARRAYSIZE(wchScheme);
		WCHAR wchHostName[1024] = { 0 };
		urlComponent.lpszHostName = wchHostName;
		urlComponent.dwHostNameLength = ARRAYSIZE(wchHostName);
		WCHAR wchUrlPath[1024] = { 0 };
		urlComponent.lpszUrlPath = wchUrlPath;
		urlComponent.dwUrlPathLength = ARRAYSIZE(wchUrlPath);
		WCHAR wchExtraInfo[1024] = { 0 };
		urlComponent.lpszExtraInfo = wchExtraInfo;
		urlComponent.dwExtraInfoLength = ARRAYSIZE(wchExtraInfo);		
		WinHttpCrackUrl(wurl.c_str(), wurl.length(), ICU_ESCAPE, &urlComponent);

		HINTERNET sessionHandle = WinHttpOpen(L"WinHttp", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
		if (sessionHandle == NULL)
		{
			printf("Invalid WinHTTP-session handle\n");
			return result;
		}
		

		HINTERNET connectionHandle = WinHttpConnect(sessionHandle, urlComponent.lpszHostName, INTERNET_DEFAULT_HTTP_PORT, 0);
		if (connectionHandle == NULL)
		{
			printf("Invalid connection handle\n");
			WinHttpCloseHandle(sessionHandle);
			return result;
		}		

		std::wstring urlPath = urlComponent.lpszUrlPath;
		if (urlComponent.dwExtraInfoLength > 0) {
			urlPath += urlComponent.lpszExtraInfo;
		}
		HINTERNET requestHandle = WinHttpOpenRequest(connectionHandle, L"GET", urlPath.c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
		if (requestHandle == NULL)
		{
			printf("Invalid request handle\n");
			WinHttpCloseHandle(connectionHandle);
			WinHttpCloseHandle(sessionHandle);
			return result;
		}			

		BOOL success = WinHttpSendRequest(requestHandle, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
		if (!success)
		{
			printf("Request failed\n");
			WinHttpCloseHandle(requestHandle);
			WinHttpCloseHandle(connectionHandle);
			WinHttpCloseHandle(sessionHandle);			
			return result;
		}			

		success = WinHttpReceiveResponse(requestHandle, NULL);
		if (!success)
		{
			// Report any errors.
			printf("Error %d has occurred.\n", GetLastError());
			WinHttpCloseHandle(requestHandle);
			WinHttpCloseHandle(connectionHandle);
			WinHttpCloseHandle(sessionHandle);
			return result;
		}
		
		DWORD dwSize;
		do
		{
			dwSize = 0;
			LPSTR pszOutBuffer;
			DWORD dwDownloaded = 0;
			if (!WinHttpQueryDataAvailable(requestHandle, &dwSize))
			{
				printf("Error %u in WinHttpQueryDataAvailable.\n", GetLastError());
				break;
			}
			// No more available data.
			if (!dwSize)
				break;
			// Allocate space for the buffer.
			pszOutBuffer = new char[dwSize + 1];
			if (!pszOutBuffer)
			{
				printf("Out of memory\n");
				break;
			}
			// Read the Data.
			ZeroMemory(pszOutBuffer, dwSize + 1);
			if (!WinHttpReadData(requestHandle, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
			{
				printf("Error %u in WinHttpReadData.\n", GetLastError());
			}
			else
			{
				result.append(pszOutBuffer);
			}
			// Free the memory allocated to the buffer.
			delete[] pszOutBuffer;
			// This condition should never be reached since WinHttpQueryDataAvailable
			// reported that there are bits to read.
			if (!dwDownloaded)
				break;
		} while (dwSize > 0);

		WinHttpCloseHandle(requestHandle);
		WinHttpCloseHandle(connectionHandle);
		WinHttpCloseHandle(sessionHandle);
		return result;
	}

};
