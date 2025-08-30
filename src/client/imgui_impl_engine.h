// dear imgui: Renderer Backend with shaders / programmatic pipeline

#pragma once
#include "imgui.h"      // IMGUI_IMPL_API
#ifndef IMGUI_DISABLE

// Follow "Getting Started" link and check examples/ folder to learn about using backends!
IMGUI_IMPL_API bool     ImGui_ImplEngine_Init(const char* glsl_version = nullptr);
IMGUI_IMPL_API void     ImGui_ImplEngine_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplEngine_NewFrame();
IMGUI_IMPL_API void     ImGui_ImplEngine_RenderDrawData(ImDrawData* draw_data);

// (Optional) Called by Init/NewFrame/Shutdown
IMGUI_IMPL_API bool     ImGui_ImplEngine_CreateFontsTexture();
IMGUI_IMPL_API void     ImGui_ImplEngine_DestroyFontsTexture();
IMGUI_IMPL_API bool     ImGui_ImplEngine_CreateDeviceObjects();
IMGUI_IMPL_API void     ImGui_ImplEngine_DestroyDeviceObjects();

#endif // #ifndef IMGUI_DISABLE
