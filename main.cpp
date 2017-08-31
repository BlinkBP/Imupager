#include <algorithm>
#include <iostream>
#include "Imupager.h"

char* getCmdOption(char** begin, char** end, const std::string & option);
bool copyToClipboard(std::string str);

int main(int argc, char * argv[])
{
	if (argc > 0)
	{
		char* path = getCmdOption(argv, argv + argc, "-f"); //getting the image path
		char* url = getCmdOption(argv, argv + argc, "-u");
		Imupager up;
		up.initializeUpload();
		//If user is sending a file
		if (path)
		{
			HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);
			DWORD dwFileSize = GetFileSize(hFile, NULL);
			void* pBuf = calloc(1, dwFileSize);
			DWORD dwBytesRead = 0;
			bool fResult = ReadFile(hFile, pBuf, dwFileSize, &dwBytesRead, NULL);

			if (fResult)
			{
				std::cout << "File loaded. Size: " << dwFileSize << std::endl;
				up.uploadFile((void*)pBuf, dwFileSize);
				Imupager up;
				std::cout << "Sending POST" << std::endl;
				up.uploadFile((void*)pBuf, dwFileSize);
				if (up.uploadSuccessful())
				{
					std::string url = up.getUrl();
					bool cSuccess = copyToClipboard(url);
					if (cSuccess)
					{
						std::cout << "URL copied to clippboard successfully." << std::endl;
					}
				}
			}
			else
				std::cout << "Could not read the file: " << GetLastError() << std::endl;
		}
		//If user is sending a url
		else if (url)
		{
			std::cout << "Url loaded." << std::endl;
			up.uploadUrl(url);
		}
		//Checking if everything was successful then copying the result url to clipboard
		if (up.uploadSuccessful())
		{
			std::string url = up.getUrl();
			bool cSuccess = copyToClipboard(url);
			if (cSuccess)
			{
				std::cout << "URL copied to clippboard successfully." << std::endl;
			}
			else
				std::cout << "Error while copying to clipboard." << std::endl;
		}
		else
			std::cout << "Upload not succesfull." << std::endl;
	}
	long close = 5000;
	std::cout << "Application will close in " << close << " miliseconds." << std::endl;
	Sleep(close);
	return 0;
}

char*  getCmdOption(char** begin, char** end, const std::string & option)
{
	char** itr = std::find(begin, end, option);
	if (itr != end && ++itr != end)
	{
		return *itr;
	}
	return 0;
}

bool copyToClipboard(std::string str)
{
	OpenClipboard(NULL);
	EmptyClipboard();
	HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, str.size());
	if (!hg)
	{
		CloseClipboard();
		return FALSE;
	}
	memcpy_s(GlobalLock(hg), str.size(), str.c_str(), str.size());
	GlobalUnlock(hg);
	SetClipboardData(CF_TEXT, hg);
	CloseClipboard();
	GlobalFree(hg);
	return TRUE;
}
