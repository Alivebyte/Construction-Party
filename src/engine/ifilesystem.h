#ifndef IFILESYSTEM_H
#define IFILESYSTEM_H

#define INVALID_FILE_HANDLE_VALUE -1

// File handle type
typedef int FileHandle_t;

// The file system interface.
class IFileSystem
{
public:
	virtual void Init() = 0;
	virtual void Shutdown() = 0;

	virtual FileHandle_t OpenFile( const char* pFilename, const char* pMode ) = 0;
	virtual void CloseFile( FileHandle_t hFile ) = 0;

	// File API

	virtual size_t GetFileLength( FileHandle_t hFile ) = 0;
	virtual void ReadFile( FileHandle_t hFile, void* pData, size_t uiSize ) = 0;
	virtual void WriteFile( FileHandle_t hFile, void* pData, size_t uiSize ) = 0;

	virtual bool IsExist( const char* pFilename ) = 0;
};

extern "C" __declspec(dllexport) IFileSystem* GetFileSystem();

#endif // !FILESYSTEM_H
