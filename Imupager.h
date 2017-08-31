#pragma once
#include <iostream>
#include <algorithm>
#include <iterator>
/*windows api*/
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

class Imupager
{
public:
	Imupager();
	~Imupager();

	void initializeUpload();
	void uploadFile(void* file, DWORD size);
	void uploadUrl(char* url);
	bool uploadSuccessful();
	std::string getUrl();

private:
	LPCWSTR	adr, host, verb;
	HINTERNET hSession, hConnect, hRequest;
	BOOL  bResults, uSuccess;
	DWORD rSize, rDownloaded;
	char* response;
	char* authHeader;
	std::string uUrl;

	bool initializeWinHTTP();
	void receiveResponse();
	void parseResponse(char* response, DWORD size);
	void printError(char* name);
};