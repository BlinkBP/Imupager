#pragma once
#include <iostream>
#include <fstream>
/*windows api*/
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

class Imupager
{
public:
	Imupager();
	~Imupager();
	void sendPOST(void* file, DWORD size);

private:
	LPCWSTR	adr;
	HINTERNET hSession, hConnect, hRequest;
	BOOL  bResults = FALSE;
	unsigned long int rMaxSize, rSize, rDownloaded;
	char* response;
	char* rInitialBoundary = "--Imupager\r\n";
	//char* rDataField = "Content-Disposition: form-data; name=\"image\";\r\nfilename=\"image.jpg\"\r\nContent-Type: image/jpeg;\r\n";
	char* rDataField = "Content-Disposition: form-data; name=\"Image\";\r\nContent-Type: image/jpeg;\r\n";
	char* rBoundary = "--Imupager--\r\n";

	void initializeWinHTTP();
	LPCWSTR convCharLPCWSTR(char* str);
	void receiveResponse();
};