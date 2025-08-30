// dear imgui: Renderer Backend with shaders / programmatic pipeline

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "imgui.h"
#ifndef IMGUI_DISABLE
#include <stdio.h>
#include <stdint.h>     // intptr_t
#if defined(__APPLE__)
#include <TargetConditionals.h>
#endif

// Clang/GCC warnings with -Weverything
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-warning-option" // warning: ignore unknown flags
#pragma clang diagnostic ignored "-Wold-style-cast"         // warning: use of old-style cast
#pragma clang diagnostic ignored "-Wsign-conversion"        // warning: implicit conversion changes signedness
#pragma clang diagnostic ignored "-Wunused-macros"          // warning: macro is not used
#pragma clang diagnostic ignored "-Wnonportable-system-include-path"
#pragma clang diagnostic ignored "-Wcast-function-type"     // warning: cast between incompatible function types (for loader)
#endif
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"                  // warning: unknown option after '#pragma GCC diagnostic' kind
#pragma GCC diagnostic ignored "-Wunknown-warning-option"   // warning: unknown warning group 'xxx'
#pragma GCC diagnostic ignored "-Wcast-function-type"       // warning: cast between incompatible function types (for loader)
#pragma GCC diagnostic ignored "-Wstrict-overflow"          // warning: assuming signed overflow does not occur when simplifying division / ..when changing X +- C1 cmp C2 to X cmp C2 -+ C1
#endif

#include "imgui_impl_engine.h"
#include "irender.h"

#define MAX_VB_SIZE sizeof(ImDrawVert) * 8192
#define MAX_IB_SIZE sizeof(ImDrawIdx) * 8192

InputLayoutDesc_t g_uiBaseInputLayout[] =
{
    { VERTEXATTR_VEC2, SHADERSEMANTIC_POSITION },
    { VERTEXATTR_VEC2, SHADERSEMANTIC_TEXCOORD },
    { VERTEXATTR_UINT, SHADERSEMANTIC_COLOR },
};

// Render Data
struct ImGui_Impl_IRender_Data
{
    ITexture2D      *FontTexture;
    IShader*        ShaderHandle;
    IGPUBuffer      *VboHandle, *ElementsHandle;
    size_t          VertexBufferSize;
    size_t          IndexBufferSize;

    ImGui_Impl_IRender_Data() { memset((void*)this, 0, sizeof(*this)); }
};

// Backend data stored in io.BackendRendererUserData to allow support for multiple Dear ImGui contexts
// It is STRONGLY preferred that you use docking branch with multi-viewports (== single Dear ImGui context + multiple windows) instead of multiple Dear ImGui contexts.
static ImGui_Impl_IRender_Data* ImGui_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_Impl_IRender_Data*)ImGui::GetIO().BackendRendererUserData : nullptr;
}

// Functions
bool    ImGui_ImplEngine_Init(const char* glsl_version)
{
    ImGuiIO& io = ImGui::GetIO();
    IMGUI_CHECKVERSION();
    IM_ASSERT(io.BackendRendererUserData == nullptr && "Already initialized a renderer backend!");

    // Setup backend capabilities flags
    ImGui_Impl_IRender_Data* bd = IM_NEW(ImGui_Impl_IRender_Data)();
    io.BackendRendererUserData = (void*)bd;
    io.BackendRendererName = "imgui_impl_opengl3";

    return true;
}

void    ImGui_ImplEngine_Shutdown()
{
    ImGui_Impl_IRender_Data* bd = ImGui_GetBackendData();
    IM_ASSERT(bd != nullptr && "No renderer backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplEngine_DestroyDeviceObjects();
    io.BackendRendererName = nullptr;
    io.BackendRendererUserData = nullptr;
    io.BackendFlags &= ~ImGuiBackendFlags_RendererHasVtxOffset;
    IM_DELETE(bd);
}

void    ImGui_ImplEngine_NewFrame()
{
    ImGui_Impl_IRender_Data* bd = ImGui_GetBackendData();
    IM_ASSERT(bd != nullptr && "Context or backend not initialized! Did you call ImGui_ImplEngine_Init()?");

    if (!bd->ShaderHandle)
        ImGui_ImplEngine_CreateDeviceObjects();
    if (!bd->FontTexture)
        ImGui_ImplEngine_CreateFontsTexture();
}

static void ImGui_ImplEngine_SetupRenderState(ImDrawData* draw_data, int fb_width, int fb_height)
{
    ImGui_Impl_IRender_Data* bd = ImGui_GetBackendData();

    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
    GetRender()->SetBlending(true);
    GetRender()->SetBlendEquation(BE_FUNC_ADD);
    GetRender()->SetBlendFuncSeparate(BF_SRC_ALPHA, BF_ONE_MINUS_SRC_ALPHA, BF_ONE, BF_ONE_MINUS_SRC_ALPHA);
    
    // Rende states
    GetRender()->SetCullFace(false);
    GetRender()->SetDepthTest(false);
    GetRender()->SetStencilTest(false);
    GetRender()->SetScissorTest(true);

#ifdef IMGUI_IMPL_OPENGL_MAY_HAVE_POLYGON_MODE
    if (bd->HasPolygonMode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

    // Bind vertex/index buffers and setup attributes for ImDrawVert
    GetRender()->SetVertexBuffer(bd->VboHandle);
    GetRender()->SetIndexBuffer(bd->ElementsHandle);

    // Setup viewport, orthographic projection matrix
    // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
    GetRender()->SetViewport(0, 0, (int)fb_width, (int)fb_height);
    float L = draw_data->DisplayPos.x;
    float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
    float T = draw_data->DisplayPos.y;
    float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
    const float ortho_projection[4][4] =
    {
        { 2.0f/(R-L),   0.0f,         0.0f,   0.0f },
        { 0.0f,         2.0f/(T-B),   0.0f,   0.0f },
        { 0.0f,         0.0f,        -1.0f,   0.0f },
        { (R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f },
    };
    GetRender()->SetShader(bd->ShaderHandle);
    GetRender()->SetUniformSampler(bd->ShaderHandle, SAMPLER_ALBEDO, 0);
    GetRender()->SetUniformMatrix(bd->ShaderHandle, UNIFORM_PROJ_MATRIX, &ortho_projection[0][0]);
}

// OpenGL3 Render function.
// Note that this implementation is little overcomplicated because we are saving/setting up/restoring every OpenGL state explicitly.
// This is in order to be able to run within an OpenGL engine that doesn't do so.
void    ImGui_ImplEngine_RenderDrawData(ImDrawData* draw_data)
{
    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0)
        return;

    ImGui_Impl_IRender_Data* bd = ImGui_GetBackendData();

    ImGui_ImplEngine_SetupRenderState(draw_data, fb_width, fb_height);

    // Will project scissor/clipping rectangles into framebuffer space
    ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
    ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

    // Render command lists
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* draw_list = draw_data->CmdLists[n];

        // Upload vertex/index buffers
        const size_t vtx_buffer_size = (size_t)draw_list->VtxBuffer.Size * (int)sizeof(ImDrawVert);
        const size_t idx_buffer_size = (size_t)draw_list->IdxBuffer.Size * (int)sizeof(ImDrawIdx);
        //if (bd->UseBufferSubData)
        //{
        //    if (bd->VertexBufferSize < vtx_buffer_size)
        //    {
        //        bd->VertexBufferSize = vtx_buffer_size;
        //        GL_CALL(glBufferData(GL_ARRAY_BUFFER, bd->VertexBufferSize, nullptr, GL_STREAM_DRAW));
        //    }
        //    if (bd->IndexBufferSize < idx_buffer_size)
        //    {
        //        bd->IndexBufferSize = idx_buffer_size;
        //        GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, bd->IndexBufferSize, nullptr, GL_STREAM_DRAW));
        //    }
        //    GL_CALL(glBufferSubData(GL_ARRAY_BUFFER, 0, vtx_buffer_size, (const GLvoid*)draw_list->VtxBuffer.Data));
        //    GL_CALL(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, idx_buffer_size, (const GLvoid*)draw_list->IdxBuffer.Data));
        //}
        //else
        {
            bd->VboHandle->UpdateBuffer(draw_list->VtxBuffer.Data, vtx_buffer_size);
            bd->ElementsHandle->UpdateBuffer(draw_list->IdxBuffer.Data, idx_buffer_size);
        }

        for (int cmd_i = 0; cmd_i < draw_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &draw_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != nullptr)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    ImGui_ImplEngine_SetupRenderState(draw_data, fb_width, fb_height);
                else
                    pcmd->UserCallback(draw_list, pcmd);
            }
            else
            {
                // Project scissor/clipping rectangles into framebuffer space
                ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x, (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
                ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x, (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);
                if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                    continue;

                // Apply scissor/clipping rectangle (Y is inverted in OpenGL)
                GetRender()->SetScissor((int)clip_min.x, (int)((float)fb_height - clip_max.y), (int)(clip_max.x - clip_min.x), (int)(clip_max.y - clip_min.y));

                // Bind texture, Draw
                GetRender()->SetTexture2D(0, (ITexture2D*)(intptr_t)pcmd->GetTexID());

                GetRender()->DrawIndexed(PT_TRIANGLES, (uint32_t)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? true : false, (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)));
            }
        }
    }
}

bool ImGui_ImplEngine_CreateFontsTexture()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui_Impl_IRender_Data* bd = ImGui_GetBackendData();

    // Build texture atlas
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bit (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

    // Upload texture to graphics system
    // (Bilinear sampling is required by default. Set 'io.Fonts->Flags |= ImFontAtlasFlags_NoBakedLines' or 'style.AntiAliasedLinesUseTex = false' to allow point/nearest sampling)
    bd->FontTexture = GetRender()->CreateTexture2D("ImGuiFontTexture", width, height, PF_R8G8B8A8, pixels, false);

    // Store identifier
    io.Fonts->SetTexID((ImTextureID)(intptr_t)bd->FontTexture);

    return true;
}

void ImGui_ImplEngine_DestroyFontsTexture()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui_Impl_IRender_Data* bd = ImGui_GetBackendData();
    if (bd->FontTexture)
    {
        delete bd->FontTexture;
        io.Fonts->SetTexID(0);
        bd->FontTexture = 0;
    }
}

bool    ImGui_ImplEngine_CreateDeviceObjects()
{
    ImGui_Impl_IRender_Data* bd = ImGui_GetBackendData();

    // Create shaders
    bd->ShaderHandle = GetRender()->CreateShader("ui_base", "data/shaders/ui_base.vs", "data/shaders/ui_base.ps",
        g_uiBaseInputLayout, sizeof(g_uiBaseInputLayout) / sizeof(g_uiBaseInputLayout[0]));

    // Create buffers
    bd->VboHandle = GetRender()->CreateVertexBuffer(nullptr, MAX_VB_SIZE, true);
    bd->ElementsHandle = GetRender()->CreateIndexBuffer(nullptr, MAX_IB_SIZE, true);

    ImGui_ImplEngine_CreateFontsTexture();

    return true;
}

void    ImGui_ImplEngine_DestroyDeviceObjects()
{
    ImGui_Impl_IRender_Data* bd = ImGui_GetBackendData();
    if (bd->VboHandle)      { delete bd->VboHandle; bd->VboHandle = 0; }
    if (bd->ElementsHandle) { delete bd->ElementsHandle; bd->ElementsHandle = 0; }
    ImGui_ImplEngine_DestroyFontsTexture();
}

//-----------------------------------------------------------------------------

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif // #ifndef IMGUI_DISABLE
