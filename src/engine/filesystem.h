#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include "ifilesystem.h"

#define MAX_OPENED_FILES 16
#define MAX_FNAME 260

struct FileHandleImpl
{
	char m_szFilename[MAX_FNAME];
	FILE* m_pFile;
};

class FileSystem : public IFileSystem
{
public:
	FileSystem(void);
	~FileSystem(void);

	void Init();
	void Shutdown();

	FileHandle_t OpenFile(const char* pFilename, const char* pMode);
	void CloseFile(FileHandle_t hFile);

	// File API

	size_t GetFileLength(FileHandle_t hFile);
	void ReadFile(FileHandle_t hFile, void* pData, size_t uiSize);
	void WriteFile(FileHandle_t hFile, void* pData, size_t uiSize);

	bool IsExist(const char* pFilename);

private:
	FileHandleImpl m_Files[MAX_OPENED_FILES];
	int m_iOpenedFiles;

};

#endif // !FILESYSTEM_H
