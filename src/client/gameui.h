#ifndef GAMEUI_H
#define GAMEUI_H

#include "irender.h"
#include "soundsystem.h"

class GameUI
{
public:
	ITexture2D* m_pCursorTex;
	ITexture2D* m_pBackgroundTex;
	ITexture2D* m_pStarTex;
	ITexture2D* m_pButtonTex;
	ITexture2D* m_pTutorialTex;

	SoundHandle_t m_hMainMenuMusic;
	SoundHandle_t m_hInGameMusic;

public:
	GameUI();
	~GameUI();

	void Init();

	void RenderMainMenu();
	void RenderHUD();
};

extern GameUI g_GameUI;

#endif // !GAMEUI_H
