// client_main.cpp - Client game initialization
#include "iengine.h"
#include "ientity.h"
#include "irender.h"

#include "gameui.h"

#include <SDL.h>

#include <imgui.h>
#include "imgui_impl_sdl2.h"
#include "imgui_impl_engine.h"

class ClientGame : public IClientGame
{
public:
	// Inherited via IClientGame
	void Init() override;
	void Shutdown() override;
	void OnEvent(const SDL_Event* pEvent) override;
	void Render() override;

	void RenderOverlay();
};

static ClientGame s_ClientGame;

void ClientGame::Init()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOther(GetEngine()->GetWindow());
	ImGui_ImplEngine_Init();

	g_GameUI.Init();
}

void ClientGame::Shutdown()
{
	ImGui_ImplEngine_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void ClientGame::OnEvent(const SDL_Event* pEvent)
{
	ImGui_ImplSDL2_ProcessEvent(pEvent); // Forward your event to backend

	UserCmd userCmd;
	memset(&userCmd, 0, sizeof(userCmd));

	if (pEvent->type == SDL_KEYDOWN)
	{
		if (pEvent->key.keysym.sym == SDLK_w)
			userCmd.walkForward = true;
		if (pEvent->key.keysym.sym == SDLK_s)
			userCmd.walkBackward = true;
		if (pEvent->key.keysym.sym == SDLK_a)
			userCmd.strafeLeft = true;
		if (pEvent->key.keysym.sym == SDLK_d)
			userCmd.strafeRight = true;
	}

	int posX = 0, posY = 0;
	SDL_GetMouseState(&posX, &posY);

	userCmd.mouseX = (uint16_t)posX;
	userCmd.mouseY = (uint16_t)posY;

	GetEngine()->GetServerGame()->SendUserCmd(&userCmd);
}

void ClientGame::Render()
{
	int width = 0, height = 0;
	SDL_GetWindowSize(GetEngine()->GetWindow(), &width, &height);

	float fAspectRatio = (float)width / (float)height;
	glm::mat4 projection = glm::perspective(glm::radians(70.0f), fAspectRatio, 0.1f, 100.0f);
	GetRender()->SetProjectionMatrix(projection);

	IEntity* pEntity = GetServerGameAPI()->FindEntityByClassname("player");
	if (pEntity)
	{
		glm::mat4 view = glm::inverse(pEntity->GetEntityMatrix());
		GetRender()->SetViewMatrix(view);
	}

	ImGui_ImplSDL2_NewFrame();
	ImGui_ImplEngine_NewFrame();
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow(); // Show demo window! :)

	//g_GameUI.Render();

	RenderOverlay();

	ImGui::Render();
	ImGui_ImplEngine_RenderDrawData(ImGui::GetDrawData());
}

void ClientGame::RenderOverlay()
{
	float height = 8.0f;

	ImDrawList* pDrawList = ImGui::GetForegroundDrawList();

	static char szBuffer[256];
	sprintf(szBuffer, "FPS: %.2f", ImGui::GetIO().Framerate);
	pDrawList->AddText(ImVec2(8.0f, height), 0xffffffff, szBuffer);
	height += 12.0f;

	int x = 0, y = 0;
	SDL_GetMouseState(&x, &y);

	sprintf(szBuffer, "mouse pos: %d %d", x, y);
	pDrawList->AddText(ImVec2(8.0f, height), 0xffffffff, szBuffer);
	height += 12.0f;

	IEntity* pEntity = GetServerGameAPI()->FindEntityByClassname("player");
	if (pEntity)
	{
		sprintf(szBuffer, "pos: %.2f %.2f %.2f", pEntity->GetOrigin().x, pEntity->GetOrigin().y, pEntity->GetOrigin().z);
		pDrawList->AddText(ImVec2(8.0f, height), 0xffffffff, szBuffer);
	}

}

// Client Export
extern "C" __declspec(dllexport) IClientGame* Client_Main()
{
	return &s_ClientGame;
}
