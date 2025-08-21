#include <stdio.h>
#include <stdarg.h>
#include <assert.h>

#include "ilogger.h"

class LoggerImpl : public ILogger
{
public:
	LoggerImpl();
	~LoggerImpl();

	// Inherited via ILogger
	void Init() override;
	void Shutdown() override;

	void Print(const char* message, ...) override;

private:
	FILE* m_pLogFile;
};

LoggerImpl::LoggerImpl() :
	m_pLogFile(nullptr)
{
}

LoggerImpl::~LoggerImpl()
{
	Shutdown();
}

void LoggerImpl::Init()
{
	if (m_pLogFile)
		return;

	m_pLogFile = fopen("engine.log", "w");
}

void LoggerImpl::Shutdown()
{
	if (m_pLogFile)
	{
		fclose(m_pLogFile);
		m_pLogFile = nullptr;
	}
}

void LoggerImpl::Print(const char* message, ...)
{
	static char buffer[1024 * 1024];
	va_list args;

	va_start(args, message);
	int len = vsprintf(buffer, message, args);
	va_end(args);

	if (buffer[len] != '\n')
	{
		buffer[len] = '\n';
		buffer[len + 1] = '\0';
	}

	if (m_pLogFile)
		fwrite(buffer, sizeof(char), len + 1, m_pLogFile);

	fwrite(buffer, 1, len + 1, stdout);
}

ILogger* GetLogger()
{
	static LoggerImpl impl;
	return &impl;
}
