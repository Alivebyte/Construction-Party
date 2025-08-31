#ifndef GAMEUI_H
#define GAMEUI_H

#include "irender.h"

class GameUI
{
public:
	ITexture2D* m_pCursorTex;
	ITexture2D* m_pBackgroundTex;
	ITexture2D* m_pStarTex;
	ITexture2D* m_pButtonTex;

public:
	GameUI();
	~GameUI();

	void Init();

	void RenderMainMenu();
};

extern GameUI g_GameUI;

#endif // !GAMEUI_H
