#include <algorithm>
#include <iostream>
#include "Imupager.h"

char*  getCmdOption(char** begin, char** end, const std::string & option);

int main(int argc, char * argv[])
{
	//if (argc > 0)
	if (true)
	{
		//char* path = getCmdOption(argv, argv + argc, "-u"); //getting the image path
		char* path = "G:\\Dropbox\\1b4c328d38f03404b6cef9b742f77c7c.jpg";

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
				up.sendPOST((void*)pBuf, dwFileSize);
			}
			else
				std::cout << "Could not read the file: " << GetLastError() << std::endl;
		}
	}
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