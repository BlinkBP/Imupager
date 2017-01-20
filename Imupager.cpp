#include "Imupager.h"

Imupager::Imupager(){
	adr = L"uploads.im";
	rMaxSize = 512;
	initializeWinHTTP();
};

Imupager::~Imupager()
{
	// Close any open handles.
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);
};

void Imupager::sendPOST(void* file, DWORD size)
{
	if (hConnect != NULL)
	{
		std::cout << "Connection established." << std::endl;
		hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/api?upload", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_REFRESH);
		if (hRequest)
		{
			std::cout << "Request opened." << std::endl;
			long length = strlen(rInitialBoundary) + strlen(rDataField) + size + strlen(rBoundary);
			bResults = WinHttpSendRequest(hRequest, L"Content-Type:multipart/form-data; boundary:ImupageR", -1L, NULL, 0, length, 0);
			if (bResults)
			{
				std::cout << "Request sent." << std::endl;
				bResults = WinHttpWriteData(hRequest, rInitialBoundary, strlen(rInitialBoundary), &rSize);
				bResults = WinHttpWriteData(hRequest, rDataField, strlen(rDataField), &rSize);
				bResults = WinHttpWriteData(hRequest, (LPVOID)file, size, &rSize);
				bResults = WinHttpWriteData(hRequest, rBoundary, strlen(rBoundary), &rSize);
				if (bResults)
				{
					receiveResponse();
				}
				else
				{
					std::cout << GetLastError() << std::endl;
				}
			}
			else
			{
				std::cout << "Error in WinHttpSendRequest: " << GetLastError() << std::endl;
			}
		}
		else
		{
			std::cout << "Error in WinHttpAddRequestHeaders: " << GetLastError() << std::endl;
		}
	}
};

void Imupager::receiveResponse()
{
	bResults = WinHttpReceiveResponse(hRequest, NULL);
	if (bResults)
	{
		do
		{
			rSize = 0;
			std::cout << "Receiving a response." << std::endl;
			if (!WinHttpQueryDataAvailable(hRequest, &rSize))
				std::cout << "Error in WinHttpQueryDataAvailable: " << GetLastError() << std::endl;
			response = new char[rSize + 1]();
			if (!WinHttpReadData(hRequest, (LPVOID)response, rSize, &rDownloaded))
				std::cout << "Error in WinHttpReadData: " << GetLastError() << std::endl;
			else
				std::cout << response << std::endl;
			delete[] response;
		} while (rSize > 0);
	}
	else
	{
		std::cout << "Error in WinHttpReceiveResponse: " << GetLastError() << std::endl;
	}
};

void Imupager::initializeWinHTTP()
{
	//initializing the use of WinHTTP functions and getting WinHTTP-session handle.
	hSession = WinHttpOpen(L"Imupager/0.5", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (hSession)
	{
		std::cout << "Session established." << std::endl;
		hConnect = WinHttpConnect(hSession, adr, INTERNET_DEFAULT_HTTP_PORT, 0);
		if (hConnect != NULL)
			std::cout << "Connection established." << std::endl;
		else
		{
			std::cout << GetLastError() << std::endl;
		}
	}
	else
	{
		std::cout << "Could not establish a session." << std::endl;
	}
};

LPCWSTR Imupager::convCharLPCWSTR(char* str)
{
	wchar_t* args = new wchar_t[strlen(str) + 1];
	size_t outSize;
	mbstowcs_s(&outSize, args, strlen(str) + 1, str, strlen(str));
	return (LPCWSTR)args;
};