#include "ilogger.h"
#include "gameui.h"
#include "ui_menu.h"

#include <SDL.h>
#include <imgui.h>

namespace ImGui
{
	bool ImageButtonWithText(const char* str_id, ImTextureID texture, const char* text, const ImVec2& size)
	{
		bool result = ImGui::ImageButton(str_id, texture, size);

		ImVec2 textSize = ImGui::CalcTextSize(text);
		ImVec2 pos = ImGui::GetItemRectMin();
		ImVec2 buttonSize = ImGui::GetItemRectSize();

		ImGui::GetWindowDrawList()->AddText(ImVec2(pos.x + (buttonSize.x - textSize.x) * 0.5f,
			pos.y + (buttonSize.y - textSize.y) * 0.5f), 0xffffffff, text);

		return result;
	}
}

GameUI g_GameUI;

GameUI::GameUI() :
	m_pCursorTex(nullptr),
	m_pStarTex(nullptr),
	m_pButtonTex(nullptr),
	m_pBackgroundTex(nullptr)
{
}

GameUI::~GameUI()
{
	m_pCursorTex = nullptr;
	m_pStarTex = nullptr;
	m_pButtonTex = nullptr;
	m_pBackgroundTex = nullptr;
}

void GameUI::Init()
{
	m_pCursorTex = GetRender()->LoadTexture2D("data/textures/ui/ui_cursor.png");
	m_pStarTex = GetRender()->LoadTexture2D("data/textures/ui/ui_star.png");
	m_pButtonTex = GetRender()->LoadTexture2D("data/textures/ui/ui_button.png");
	m_pBackgroundTex = GetRender()->LoadTexture2D("data/textures/ui/ui_background.png");
}

void GameUI::RenderMainMenu()
{
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
	ImGui::SetNextWindowBgAlpha(0.0f);

	if (ImGui::Begin("mainmenu", 0, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
	{
		// lazy to do in proper way, so hack it
		ImGui::GetWindowDrawList()->AddImage((ImTextureID)m_pBackgroundTex, ImVec2(0.f, 0.f), ImGui::GetIO().DisplaySize);

		int position = (int)ImGui::GetIO().DisplaySize.y - 370 - 25;/*SAFE RECT*/

		ImGui::SetCursorPosX(50);

		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), position));

		if (ImGui::ImageButtonWithText("new_game_btn", (ImTextureID)m_pButtonTex, "New Game", ImVec2(m_pButtonTex->GetWidth(), m_pButtonTex->GetHeight())))
		{
			GetLogger()->Print("New Game ...");
		}
		
		ImGui::SetCursorPosX(50);
		
		if (ImGui::ImageButtonWithText("set_game_btn", (ImTextureID)m_pButtonTex, "Settings", ImVec2(m_pButtonTex->GetWidth(), m_pButtonTex->GetHeight())))
		{
			g_ui_menu_options.open();
		}
		
		ImGui::SetCursorPosX(50);
		
		if (ImGui::ImageButtonWithText("quit_game_btn", (ImTextureID)m_pButtonTex, "Quit", ImVec2(m_pButtonTex->GetWidth(), m_pButtonTex->GetHeight())))
		{
			SDL_Event event;
			memset(&event, 0, sizeof(event));
			event.type = SDL_QUIT;
			SDL_PushEvent(&event);
		}
	}

	ImGui::End();

	g_ui_menu_options.draw();

	//ImGui::Begin("UI Zoo");
	//ImGui::Image((ImTextureID)m_pCursorTex, ImVec2(m_pCursorTex->GetWidth(), m_pCursorTex->GetHeight())); ImGui::SameLine();
	//ImGui::Image((ImTextureID)m_pStarTex, ImVec2(m_pStarTex->GetWidth(), m_pStarTex->GetHeight())); ImGui::SameLine();
	//ImGui::Image((ImTextureID)m_pButtonTex, ImVec2(m_pButtonTex->GetWidth(), m_pButtonTex->GetHeight()));
	//ImGui::Text("Extended:");

	//ImGui::ImageButtonWithText("new_game_btn", (ImTextureID)m_pButtonTex, "New Game",
	//	ImVec2(m_pButtonTex->GetWidth(), m_pButtonTex->GetHeight()));

	//ImGui::ImageButtonWithText("quit_game_btn", (ImTextureID)m_pButtonTex, "Quit",
	//	ImVec2(m_pButtonTex->GetWidth(), m_pButtonTex->GetHeight()));

	//ImGui::End();

	SDL_ShowCursor(false);

	int x = 0, y = 0;
	SDL_GetMouseState(&x, &y);

	ImGui::GetForegroundDrawList()->AddImage((ImTextureID)m_pCursorTex, ImVec2(x, y),
		ImVec2(x + m_pCursorTex->GetWidth(), y + m_pCursorTex->GetHeight()));
}