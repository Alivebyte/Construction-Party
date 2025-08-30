// client_main.cpp - Client game initialization
#include "iengine.h"
#include "ientity.h"

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

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOther(GetEngine()->GetWindow());
	ImGui_ImplEngine_Init();
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
}

void ClientGame::Render()
{
	ImGui_ImplSDL2_NewFrame();
	ImGui_ImplEngine_NewFrame();
	ImGui::NewFrame();
	ImGui::ShowDemoWindow(); // Show demo window! :)

	ImGui::Render();
	ImGui_ImplEngine_RenderDrawData(ImGui::GetDrawData());
}

// Client Export
extern "C" __declspec(dllexport) IClientGame* Client_Main()
{
	return &s_ClientGame;
}
