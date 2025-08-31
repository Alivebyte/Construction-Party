#include <SDL.h>
#include <tinyxml2.h>
#include <imgui.h>
#include "ui_menu.h"
#include "soundsystem.h"

#define DISABLE_HZ

static const char* get_video_mode_string( int index ) {
    static char buf[64];

    video_mode& mode = g_video_mode_mgr.m_video_modes[index];
#ifndef DISABLE_HZ
    snprintf( buf, sizeof( buf ), "%ix%i %i Hz", mode.width, mode.height, mode.refresh_rate );
#else
    snprintf( buf, sizeof( buf ), "%ix%i", mode.width, mode.height );
#endif // !DISABLE_HZ

    return buf;
}

//---------------------------------------------------------------------------

video_mode_mgr g_video_mode_mgr;

video_mode_mgr::video_mode_mgr() {
    memset( m_video_modes, 0, sizeof( m_video_modes ) );
    m_num_video_modes = 0;
}

void video_mode_mgr::init() {
    int               main_display;
    int               display_count;
    SDL_DisplayMode   display_mode;
    int               display_mode_count;

    display_count = SDL_GetNumVideoDisplays();

    SDL_LogVerbose( APP_CATEGORY_UI, "Num displays: %d\n", display_count );

    for ( int i = 0; i < display_count; i++ ) {
        // finish someday uwu
        //
        // list video modes
        // display_modes = SDL_GetFullscreenDisplayModes( i, &display_mode_count );
        // if (display_modes == NULL) {
        //    SDL_LogError( LOG_CATEGORY_APP, "%s: %" SDL_PRIu32 "\n", SDL_GetError(), i );
        //}

        // SDL_LogVerbose( LOG_CATEGORY_APP, "Display with ID %" SDL_PRIu32 " has %" SDL_PRIu32 " display modes\n", displays[i], display_mode_count );

        // for ( int j = 0; j < display_mode_count; j++ ) {
        //     //
        //     display_mode = display_modes[j];
        // }
    }

    main_display = 0;

    display_mode_count = SDL_GetNumDisplayModes(main_display);

    // list video modes
    if ( display_mode_count == 0 ) {
        // error happened
        SDL_LogError( APP_CATEGORY_UI, "%s: %" SDL_PRIu32 "\n", SDL_GetError(), main_display );
    }

    SDL_LogVerbose( APP_CATEGORY_UI, "Primary display has %" SDL_PRIu32 " display modes\n", display_mode_count );

    for ( int i = 0; i < display_mode_count; i++ ) {
        SDL_GetDisplayMode(main_display, i, &display_mode);

#ifdef DISABLE_HZ
        if (display_mode.refresh_rate != 60)
            continue;
#endif // DISABLE_HZ

        SDL_LogVerbose( APP_CATEGORY_UI, "%d: %dx%d (%d)\n", i, 
            display_mode.w, display_mode.h, display_mode.refresh_rate);

        video_mode& mode  = m_video_modes[m_num_video_modes++];
        mode.width        = display_mode.w;
        mode.height       = display_mode.h;
        mode.refresh_rate = display_mode.refresh_rate;
    }

    SDL_LogVerbose( APP_CATEGORY_UI, "Total %" SDL_PRIu32 " display modes\n", m_num_video_modes );
}

int video_mode_mgr::get_video_mode_index(int width, int height, int refresh_rate)
{
    for (int i = 0; i < m_num_video_modes; i++)
    {
        if (m_video_modes[i].width == width && m_video_modes[i].height == height &&
            m_video_modes[i].refresh_rate == refresh_rate)
            return i;
    }

    return 0;
}

//---------------------------------------------------------------------------

options_config g_options_config;

options_config::options_config() {
    memset( &m_video_mode, 0, sizeof( m_video_mode ) );
    m_sfx_volume        = 0.0f;
    m_music_volume      = 0.0f;
    m_mouse_sensitive   = 0.0f;
    m_mouse_invert_axis = false;
    m_borderless        = false;
}

void options_config::init_default() {
    m_video_mode.width        = 800;
    m_video_mode.height       = 600;
    m_video_mode.refresh_rate = 60;
    m_sfx_volume              = 0.7f;
    m_music_volume            = 0.5f;
    m_mouse_sensitive         = 0.3f;
    m_borderless              = false;
}

void options_config::load() {
    FILE* fp = fopen( "config.xml", "rb" );
    if ( !fp ) {
        SDL_LogVerbose( APP_CATEGORY_UI, "failed to load config.xml, loading defaults ...\n" );
        init_default();
        save();
        return;
    }

    fclose( fp );

    tinyxml2::XMLDocument doc;
    tinyxml2::XMLError    error = doc.LoadFile( "config.xml" );
    if ( error != tinyxml2::XML_SUCCESS ) {
        SDL_Log( "Failed to parse %s. %s\n", "config.xml", doc.ErrorStr() );
        return;
    }

    tinyxml2::XMLElement* display = doc.FirstChildElement( "display_config" );
    if ( display ) {
        tinyxml2::XMLElement* width        = display->FirstChildElement( "width" );
        tinyxml2::XMLElement* height       = display->FirstChildElement( "height" );
        tinyxml2::XMLElement* refresh_rate = display->FirstChildElement( "refresh_rate" );
        tinyxml2::XMLElement* borderless   = display->FirstChildElement( "borderless" );

        if ( width ) width->QueryIntText( &m_video_mode.width );
        if ( height ) height->QueryIntText( &m_video_mode.height );
        if ( refresh_rate ) refresh_rate->QueryIntText( &m_video_mode.refresh_rate );
        if ( borderless ) borderless->QueryBoolText( &m_borderless );
    }

    tinyxml2::XMLElement* input = doc.FirstChildElement( "input_config" );
    if ( input ) {
        tinyxml2::XMLElement* mouse_sensitive = input->FirstChildElement( "mouse_sensitive" );
        tinyxml2::XMLElement* mouse_invert    = input->FirstChildElement( "mouse_invert" );

        if ( mouse_sensitive ) mouse_sensitive->QueryFloatText( &m_mouse_sensitive );
        if ( mouse_invert ) mouse_invert->QueryBoolText( &m_mouse_invert_axis );
    }

    tinyxml2::XMLElement* sound = doc.FirstChildElement( "sound_config" );
    if ( sound ) {
        tinyxml2::XMLElement* sfx_volume = sound->FirstChildElement( "sfx_volume" );
        tinyxml2::XMLElement* music_volume = sound->FirstChildElement( "music_volume" );

        if ( sfx_volume ) sfx_volume->QueryFloatText( &m_sfx_volume );
        if ( music_volume ) music_volume->QueryFloatText( &m_music_volume );
    }
}

void options_config::save() {
    char                  buf[64];
    tinyxml2::XMLDocument doc;

    // save display settings

    tinyxml2::XMLElement* display = doc.NewElement( "display_config" );
    doc.LinkEndChild( display );

    tinyxml2::XMLElement* width = display->InsertNewChildElement( "width" );
    width->SetText( m_video_mode.width );

    tinyxml2::XMLElement* height = display->InsertNewChildElement( "height" );
    height->SetText( m_video_mode.height );

    tinyxml2::XMLElement* refresh_rate = display->InsertNewChildElement( "refresh_rate" );
    refresh_rate->SetText( m_video_mode.refresh_rate );

    tinyxml2::XMLElement* borderless = display->InsertNewChildElement( "borderless" );
    borderless->SetText( m_borderless );

    // save input config

    tinyxml2::XMLElement* input = doc.NewElement( "input_config" );
    doc.LinkEndChild( input );

    tinyxml2::XMLElement* mouse_sensitive = input->InsertNewChildElement( "mouse_sensitive" );
    mouse_sensitive->SetText( m_mouse_sensitive );

    tinyxml2::XMLElement* mouse_invert = input->InsertNewChildElement( "mouse_invert" );
    mouse_invert->SetText( m_mouse_invert_axis );

    // save sound settings

    tinyxml2::XMLElement* sound = doc.NewElement( "sound_config" );
    doc.LinkEndChild( sound );

    tinyxml2::XMLElement* sfx_volume = sound->InsertNewChildElement( "sfx_volume" );
    sfx_volume->SetText( m_sfx_volume );

    tinyxml2::XMLElement* music_volume = sound->InsertNewChildElement( "music_volume" );
    music_volume->SetText( m_music_volume );

    tinyxml2::XMLError error = doc.SaveFile( "config.xml" );
    if ( error != tinyxml2::XML_SUCCESS ) {
        SDL_LogError( APP_CATEGORY_UI, "failed to save XML file '%s' error: '%s'", "config.xml", doc.ErrorStr() );
        return;
    }
}

//---------------------------------------------------------------------------

ui_menu_options g_ui_menu_options;

ui_menu_options::ui_menu_options() {
    m_show_settings_menu = false;
    m_show_apply_window  = false;
}

ui_menu_options::~ui_menu_options() {}

void ui_menu_options::draw() {
    if ( m_show_settings_menu == false ) return;

    static int item_current_idx = g_video_mode_mgr.get_video_mode_index(
    g_options_config.m_video_mode.width,
    g_options_config.m_video_mode.height,
    g_options_config.m_video_mode.refresh_rate);  // Here we store our selection data as an index.

    ImGui::Begin( "Settings", &m_show_settings_menu );

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if ( ImGui::BeginTabBar( "MyTabBar", tab_bar_flags ) ) {
        if ( ImGui::BeginTabItem( "Video" ) ) {
            if ( g_video_mode_mgr.is_inited() != false ) {
                const char* combo_preview_value = get_video_mode_string( item_current_idx );
                if ( ImGui::BeginCombo( "Video Mode", combo_preview_value ) ) {
                    for ( int i = 0; i < g_video_mode_mgr.m_num_video_modes; i++ ) {
                        const bool  is_selected       = ( item_current_idx == i );
                        const char* video_mode_string = get_video_mode_string( i );

                        if ( ImGui::Selectable( video_mode_string, is_selected ) ) item_current_idx = i;

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if ( is_selected ) ImGui::SetItemDefaultFocus();
                    }

                    ImGui::EndCombo();
                }
            }

            ImGui::EndTabItem();

            ImGui::Checkbox( "Borderless Window", &g_options_config.m_borderless );

            if ( ImGui::Button( "Apply" ) ) {
                // VideoModeManager::Apply( item_current_idx );
                m_show_apply_window = true;
            }
        }
        if ( ImGui::BeginTabItem( "Sound" ) ) {
            //ImGui::SliderFloat( "SFX Volume", &g_options_config.m_sfx_volume, 0.0f, 1.0f );
            //ImGui::SliderFloat( "Music Volume", &g_options_config.m_music_volume, 0.0f, 1.0f );

            ImGui::SliderFloat("Master Volume", &g_options_config.m_music_volume, 0.0f, 1.0f);

            g_SoundSystem.SetMasterVolume(g_options_config.m_music_volume);

            ImGui::EndTabItem();
        }
        if ( ImGui::BeginTabItem( "Input" ) ) {
            ImGui::SliderFloat( "Mouse Sensitivity", &g_options_config.m_mouse_sensitive, 0.1f, 10.0f );
            ImGui::Checkbox( "Mouse Invert", &g_options_config.m_mouse_invert_axis );
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    ImGui::End();

    if ( m_show_apply_window ) {
        ImGui::Begin( "Warning!", &m_show_apply_window, ImGuiWindowFlags_NoSavedSettings );
        ImGui::Text( "To apply the settings, you need to restart the application." );
        if (ImGui::Button("OK")) {
            m_show_apply_window = false;
            g_options_config.m_video_mode = g_video_mode_mgr.m_video_modes[item_current_idx];
        }
        ImGui::End();
    }
}

