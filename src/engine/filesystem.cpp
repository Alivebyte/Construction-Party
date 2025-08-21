#include "filesystem.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <io.h>

FileSystem::FileSystem()
{
	m_iOpenedFiles = 0;

	memset(m_Files, 0, sizeof(m_Files));
}

FileSystem::~FileSystem()
{
}

void FileSystem::Init()
{
}

void FileSystem::Shutdown()
{
	// find already opened file
	for (int i = 0; i < MAX_OPENED_FILES; i++)
	{
		if (m_Files[i].m_pFile)
		{
			fclose(m_Files[i].m_pFile);
			m_Files[i].m_pFile = nullptr;
		}
	}
}

FileHandle_t FileSystem::OpenFile(const char* pFilename, const char* pMode)
{
	int iHandle;

	// find already opened file
	for (int i = 0; i < MAX_OPENED_FILES; i++)
	{
		if (m_Files[i].m_pFile && strcmp(m_Files[i].m_szFilename, pFilename) == 0)
			return i;
	}

	if (!IsExist(pFilename) && pMode[0] == 'r')
	{
		printf("FileSystem::OpenFile: '%s' is not exist\n", pFilename);
		return -1;
	}

	// Find free handle
	for (int i = 0; i < MAX_OPENED_FILES; i++)
	{
		if (m_Files[i].m_pFile == NULL)
		{
			iHandle = i;
			break;
		}
	}

	m_Files[iHandle].m_pFile = fopen(pFilename, pMode);
	assert(m_Files[iHandle].m_pFile);

	m_iOpenedFiles++;

	return iHandle;
}

void FileSystem::CloseFile(FileHandle_t hFile)
{
	assert(hFile <= MAX_OPENED_FILES);

	fclose(m_Files[hFile].m_pFile);
	m_Files[hFile].m_pFile = NULL;

	m_iOpenedFiles--;
}

size_t FileSystem::GetFileLength(FileHandle_t hFile)
{
	assert(hFile <= MAX_OPENED_FILES);
	assert(m_Files[hFile].m_pFile);

	size_t size;

	fseek(m_Files[hFile].m_pFile, 0, SEEK_END);
	size = (size_t)ftell(m_Files[hFile].m_pFile);
	fseek(m_Files[hFile].m_pFile, 0, SEEK_SET);

	return size;
}

void FileSystem::ReadFile(FileHandle_t hFile, void* pData, size_t uiSize)
{
	assert(hFile <= MAX_OPENED_FILES);
	assert(m_Files[hFile].m_pFile);

	fread(pData, uiSize, 1, m_Files[hFile].m_pFile);
}

void FileSystem::WriteFile(FileHandle_t hFile, void* pData, size_t uiSize)
{
	assert(hFile <= MAX_OPENED_FILES);
	assert(m_Files[hFile].m_pFile);

	fwrite(pData, uiSize, 1, m_Files[hFile].m_pFile);
}

bool FileSystem::IsExist(const char* pFilename)
{
	return _access(pFilename, /*F_OK*/ 0) == 0;
}

extern "C" __declspec(dllexport) IFileSystem* GetFileSystem()
{
	static FileSystem fileSystem;
	return &fileSystem;
}