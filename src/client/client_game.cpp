// client_main.cpp - Client game initialization
#include "ilogger.h"
#include "iengine.h"
#include "ientity.h"
#include "irender.h"

#include "gameui.h"
#include "ui_menu.h"

#include "camera.h"

#include <SDL.h>

#include <imgui.h>
#include "imgui_impl_sdl2.h"
#include "imgui_impl_engine.h"

void SDLCALL Engine_LogOutputFunction(void* userdata, int category, SDL_LogPriority priority, const char* message)
{
	const char* str_cat = "";
	switch (category) {
	case APP_CATEGORY_UI:
		str_cat = "UI";
		break;
	default:
		str_cat = "";
		break;
	}

	const char* str_prior = "";
	switch (priority) {
	case SDL_LOG_PRIORITY_VERBOSE:
		str_prior = "VERBOSE";
		break;
	case SDL_LOG_PRIORITY_DEBUG:
		str_prior = "DEBUG";
		break;
	case SDL_LOG_PRIORITY_INFO:
		str_prior = "";
		break;
	case SDL_LOG_PRIORITY_WARN:
		str_prior = "WARN";
		break;
	case SDL_LOG_PRIORITY_ERROR:
		str_prior = "ERROR";
		break;
	case SDL_LOG_PRIORITY_CRITICAL:
		str_prior = "CRITICAL";
		break;
	default:
		break;
	}

	static char buffer[1024];
	sprintf(buffer, "%s (%s): %s", str_prior, str_cat, message);
	GetLogger()->Print(buffer);
}

class ClientGame : public IClientGame
{
public:
	// Inherited via IClientGame
	void Init() override;
	void Shutdown() override;
	void OnEvent(const SDL_Event* pEvent) override;
	void Render() override;

	void RenderOverlay();

private:
	Camera m_Camera;
	bool m_bShowMenu = false;
};

static ClientGame s_ClientGame;

void ClientGame::Init()
{
	// Setup custom log function
	SDL_LogSetPriority(APP_CATEGORY_UI, SDL_LOG_PRIORITY_VERBOSE);
	SDL_LogSetOutputFunction(Engine_LogOutputFunction, nullptr);

	// Initialize video manager
	g_video_mode_mgr.init();

	// Initialize config
	g_options_config.load();

	g_SoundSystem.SetMasterVolume(g_options_config.m_music_volume);

	// Resize window with config values
	SDL_SetWindowSize(GetEngine()->GetWindow(), g_options_config.m_video_mode.width, g_options_config.m_video_mode.height);
	SDL_SetWindowPosition(GetEngine()->GetWindow(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

	SDL_SetWindowBordered(GetEngine()->GetWindow(), (SDL_bool)!g_options_config.m_borderless);

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

	g_options_config.save();
}

void ClientGame::OnEvent(const SDL_Event* pEvent)
{
	ImGui_ImplSDL2_ProcessEvent(pEvent); // Forward your event to backend

	// Game ui stuff
	if (pEvent->type == SDL_KEYDOWN)
	{
		if (pEvent->key.keysym.sym == SDLK_ESCAPE)
			m_bShowMenu = !m_bShowMenu;
	}

	if (m_bShowMenu)
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}
	else
	{
	//	SDL_SetRelativeMouseMode(SDL_TRUE);
	}

	// UserCmd stuff

	m_Camera.LookAt(glm::vec3(-0.531249285f, -0.453990191f, 0.715309143f));

	if (!m_bShowMenu)
	{
		// becase of dead lines, no more control :(
		static UserCmd userCmd;
		static bool firstTime = true;
		if (firstTime)
		{
			memset(&userCmd, 0, sizeof(userCmd));
			firstTime = false;
		}

		int width = 0, height = 0;
		SDL_GetWindowSize(GetEngine()->GetWindow(), &width, &height);


		// Send mouse to player
		int posX = 0, posY = 0;
		SDL_GetMouseState(&posX, &posY);
		userCmd.mouseX = (int16_t)posX;
		userCmd.mouseY = (int16_t)posY;

		if (pEvent->type == SDL_MOUSEBUTTONDOWN)
		{
			if (pEvent->button.button == SDL_BUTTON_LEFT)
				userCmd.action = true;
		}
		else if (pEvent->type == SDL_MOUSEBUTTONUP)
		{
			if (pEvent->button.button == SDL_BUTTON_LEFT)
				userCmd.action = false;
		}

		// Send camera direction
		//userCmd.dirx = m_Camera.GetDirection().x;
		//userCmd.diry = m_Camera.GetDirection().y;
		//userCmd.dirz = m_Camera.GetDirection().z;

		// matrices
		float fAspectRatio = (float)width / (float)height;
		glm::mat4 projection = glm::perspective(glm::radians(70.0f), fAspectRatio, 0.1f, 100.0f);
		glm::mat4 view = glm::mat4(1.0f);

		IEntity* pEntity = GetServerGameAPI()->FindEntityByClassname("player");
		if (pEntity)
		{
			view = m_Camera.GetViewMatrix();
		}

		// Project mouse to the 
		glm::vec2 ndc;
		ndc.x = (2.0f * (float)posX) / (float)width -1.0f;
		ndc.y = 1.0f - (2.0f * (float)posY) / (float)height; // flip Y

		glm::vec4 nearPointNDC(ndc, -1.0f, 1.0f);
		glm::vec4 farPointNDC(ndc, 1.0f, 1.0f);

		glm::mat4 invVP = glm::inverse(projection * view);
		glm::vec4 nearPointWorld = invVP * nearPointNDC;
		glm::vec4 farPointWorld = invVP * farPointNDC;
		nearPointWorld /= nearPointWorld.w;
		farPointWorld /= farPointWorld.w;

		userCmd.posx = nearPointWorld.x;
		userCmd.posy = nearPointWorld.y;
		userCmd.posz = nearPointWorld.z;

		glm::vec3 direction = glm::normalize(glm::vec3(farPointWorld - nearPointWorld));
		userCmd.dirx = direction.x;
		userCmd.diry = direction.y;
		userCmd.dirz = direction.z;

		GetEngine()->GetServerGame()->SendUserCmd(&userCmd);

		//if (pEvent->type == SDL_KEYDOWN)
		//{
		//	if (pEvent->key.keysym.sym == SDLK_w)
		//		userCmd.walkForward = true;
		//	if (pEvent->key.keysym.sym == SDLK_s)
		//		userCmd.walkBackward = true;
		//	if (pEvent->key.keysym.sym == SDLK_a)
		//		userCmd.strafeLeft = true;
		//	if (pEvent->key.keysym.sym == SDLK_d)
		//		userCmd.strafeRight = true;
		//}
		//else if (pEvent->type == SDL_KEYUP)
		//{
		//	if (pEvent->key.keysym.sym == SDLK_w)
		//		userCmd.walkForward = false;
		//	if (pEvent->key.keysym.sym == SDLK_s)
		//		userCmd.walkBackward = false;
		//	if (pEvent->key.keysym.sym == SDLK_a)
		//		userCmd.strafeLeft = false;
		//	if (pEvent->key.keysym.sym == SDLK_d)
		//		userCmd.strafeRight = false;
		//}

		//int posX = 0, posY = 0;
		//SDL_GetMouseState(&posX, &posY);

		//int width = 0, height = 0;
		//SDL_GetWindowSize(GetEngine()->GetWindow(), &width, &height);

		//int centerX = width / 2;
		//int centerY = height / 2;

		//int deltaX = posX - centerX;
		//int deltaY = posY - centerY;

		//SDL_WarpMouseInWindow(GetEngine()->GetWindow(), centerX, centerY);
	
		//userCmd.mouseX = (int16_t)posX;
		//userCmd.mouseY = (int16_t)posY;

		//userCmd.deltaX = (int16_t)deltaX;
		//userCmd.deltaY = (int16_t)deltaY;

		//// calculate yaw and pitch
		//static float yaw = 0.0f, pitch = 0.0f;

		//yaw += (float)deltaX * g_options_config.m_mouse_sensitive;
		//pitch += (float)-deltaY * g_options_config.m_mouse_sensitive;
		//if (pitch >= 89.0f)
		//	pitch = 89.0f;
		//else if (pitch <= -89.0f)
		//	pitch = -89.0f;

		//m_Camera.SetYawPitch(yaw, pitch);

		//userCmd.dirx = m_Camera.GetDirection().x;
		//userCmd.diry = m_Camera.GetDirection().y;
		//userCmd.dirz = m_Camera.GetDirection().z;

		//GetEngine()->GetServerGame()->SendUserCmd(&userCmd);
	}
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
		m_Camera.SetPosition(pEntity->GetOrigin());

		glm::mat4 view = m_Camera.GetViewMatrix();
		GetRender()->SetViewMatrix(view);
	}

	ImGui_ImplSDL2_NewFrame();
	ImGui_ImplEngine_NewFrame();
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow(); // Show demo window! :)

	if (m_bShowMenu)
		g_GameUI.RenderMainMenu();
	else
		g_GameUI.RenderHUD();

	g_GameUI.RenderCursor();

	//RenderOverlay();

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

	sprintf(szBuffer, "delta: %f", GetEngine()->GetDeltaTime());
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
