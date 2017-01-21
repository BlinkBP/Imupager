#include "Imupager.h"

Imupager::Imupager(){
	adr = L"api.imgur.com";
	//adr = L"uploads.im";
	bResults = FALSE;
	uSuccess = FALSE;
};

Imupager::~Imupager()
{
	// Close any open handles.
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);
};

void Imupager::upload(void* file, DWORD size)
{
	bResults = FALSE;
	uSuccess = FALSE;
	bResults = initializeWinHTTP();
	if (bResults)
	{
		std::cout << "Connection established." << std::endl;

		hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/3/image", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_REFRESH);
		//hRequest = WinHttpOpenRequest(hConnect, L"POST", L"/api?upload", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_REFRESH);

		if (hRequest)
		{
			std::cout << "Request opened." << std::endl;
			//Imgur Client ID authorization header
			bResults = WinHttpAddRequestHeaders(hRequest, L"Authorization: Client-ID _PLACEHOLDER_", -1L, WINHTTP_ADDREQ_FLAG_ADD);
			if (bResults)
			{
				bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, NULL, 0, size, 0);
				if (bResults)
				{
					std::cout << "Request sent." << std::endl;

					bResults = WinHttpWriteData(hRequest, (LPVOID)file, size, &rSize);
					if (bResults)
						receiveResponse();
					else
						std::cout << "Error in WinHttpWriteData: " << GetLastError() << std::endl;
				}
				else
					std::cout << "Error in WinHttpSendRequest: " << GetLastError() << std::endl;
			}
			else
				std::cout << "Error in WinHttpAddRequestHeaders: " << GetLastError() << std::endl;
		}
		else
			std::cout << "Error in WinHttpAddRequestHeaders: " << GetLastError() << std::endl;
	}
}
bool Imupager::uploadSuccessful()
{
	return uSuccess;
}
std::string Imupager::getUrl()
{
	return uUrl;
}
;

void Imupager::receiveResponse()
{
	bResults = WinHttpReceiveResponse(hRequest, NULL);
	if (bResults)
	{
		std::cout << "Receiving a response." << std::endl;

		bResults = WinHttpQueryDataAvailable(hRequest, &rSize);
		if (bResults)
		{
			response = new char[rSize]();
			if (!WinHttpReadData(hRequest, (LPVOID)response, rSize, &rDownloaded))
				std::cout << "Error in WinHttpReadData: " << GetLastError() << std::endl;
			else
				parseResponse(response, rDownloaded);
				//std::cout << response << std::endl;
			delete[] response;
		}
		else
			std::cout << "Error in WinHttpQueryDataAvailable: " << GetLastError() << std::endl;
	}
	else
		std::cout << "Error in WinHttpReceiveResponse: " << GetLastError() << std::endl;
}

void Imupager::parseResponse(char* response, DWORD size)
{
	std::string r(response, size);
	size_t succ = r.find("\"success\":true");
	if (succ != std::string::npos)
	{
		//Upload successful
		//Let's get the url
		//Response looks like this: [...],"link":"http:\/\/i.imgur[...]"},[...]
		size_t lBegin = r.find("link") + strlen("link\":\"");
		//Searching for the ending '"' of link
		size_t lEnd = r.find('"', lBegin);
		if (lEnd != std::string::npos)
		{
			std::string url = std::string(r, lBegin, lEnd - lBegin + 1);
			//Let's sanitize the url a little; '\' characters have to go
			std::string sanitizedUrl;
			std::remove_copy(url.begin(), url.end(), std::back_inserter(sanitizedUrl), '\\');

			uUrl = std::string(sanitizedUrl);
			uSuccess = TRUE;
		}
		else
			std::cout << "Error while parsing the response." << std::endl;
	}
	else
	{
		std::cout << "Invalid response from server: " << std::endl << response << std::endl;
	}
}

bool Imupager::initializeWinHTTP()
{
	//initializing the use of WinHTTP functions and getting WinHTTP-session handle.
	hSession = WinHttpOpen(L"Imupager/1.0", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (hSession)
	{
		std::cout << "Session established." << std::endl;
		hConnect = WinHttpConnect(hSession, adr, INTERNET_DEFAULT_HTTP_PORT, 0);
		if (hConnect != NULL)
		{
			std::cout << "Connection established." << std::endl;
			return true;
		}
		else
			std::cout << "Error in WinHttpConnect: " << GetLastError() << std::endl;
	}
	else
		std::cout << "Could not establish a session." << std::endl;
	return false;
};