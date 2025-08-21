#ifndef IENGINE_H
#define IENGINE_H

class IEngine
{
public:
	virtual void Init() = 0;
	virtual void RunLoop() = 0;
	virtual void Loop() = 0;
	virtual void Shutdown() = 0;
};

#endif