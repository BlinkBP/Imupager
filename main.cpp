#include <algorithm>
#include <iostream>
#include "Imupager.h"

char* getCmdOption(char** begin, char** end, const std::string & option);
bool copyToClipboard(std::string str);

int main(int argc, char * argv[])
{
	if (argc > 0)
	{
		char* path = getCmdOption(argv, argv + argc, "-u"); //getting the image path

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
				Imupager up;
				std::cout << "Sending POST" << std::endl;
				up.upload((void*)pBuf, dwFileSize);
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
