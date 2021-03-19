#include <winhttp.h>
#include <Windows.h>
#include <winsock2.h>
#include "MHttpTools.h"
#pragma comment(lib, "winhttp.lib")

namespace MHttpTools {
	std::string GetHttp(const char* url) 
	{
		HINTERNET sessionHandle = WinHttpOpen(L"WinHttp Example", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
		if (sessionHandle)
		{
			HINTERNET connectionHandle = WinHttpConnect(sessionHandle, L"v1.hitokoto.cn", INTERNET_DEFAULT_HTTP_PORT, 0);
			if (connectionHandle)
			{
				HINTERNET requestHandle = WinHttpOpenRequest(connectionHandle, L"GET", L"/?c=f&encode=text", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
				if (requestHandle)
				{
					BOOL success = WinHttpSendRequest(requestHandle, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
					if (success)
					{
						success = WinHttpReceiveResponse(requestHandle, NULL);
						if (success)
						{
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
									printf("%s", pszOutBuffer);
								}
								// Free the memory allocated to the buffer.
								delete[] pszOutBuffer;
								// This condition should never be reached since WinHttpQueryDataAvailable
								// reported that there are bits to read.
								if (!dwDownloaded)
									break;
							} while (dwSize > 0);
						}
						else
						{
							// Report any errors.
							printf("Error %d has occurred.\n", GetLastError());
						}
					}
					else
					{
						printf("Request failed\n");
					}
					WinHttpCloseHandle(requestHandle);
				}
				else
				{
					printf("Invalid request handle\n");
				}
				WinHttpCloseHandle(connectionHandle);
			}
			else
			{
				printf("Invalid connection handle\n");
			}
			WinHttpCloseHandle(sessionHandle);
		}
		else
		{
			printf("Invalid WinHTTP-session handle\n");
		}
	}

};
