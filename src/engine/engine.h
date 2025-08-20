#ifndef ENGINE_H
#define ENGINE_H

#include "iengine.h"


class Engine : public IEngine
{
public:
	void Init() override;
	void RunLoop() override;
	void Loop() override;
	void Shutdown() override;
};

#endif