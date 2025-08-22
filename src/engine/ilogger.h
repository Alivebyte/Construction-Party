#ifndef ILOGGER_H
#define ILOGGER_H

#include <stdarg.h>

class ILogger
{
public:
	virtual void Init() = 0;
	virtual void Shutdown() = 0;

	virtual void Print(const char* message, ...) = 0;
	virtual void Error(const char* message, ...) = 0;
};

extern "C" __declspec(dllexport) ILogger* GetLogger();


#endif // !ILOGGER_H
