#ifndef UI_MENU_H
#define UI_MENU_H

#include <SDL.h>

//---------------------------------------------------------------------------

enum
{
    APP_CATEGORY_UI = SDL_LOG_CATEGORY_CUSTOM
};

#define MAX_VIDEO_MODES 128

struct video_mode {
    int  width;
    int  height;
    int  refresh_rate;
};

class video_mode_mgr {
public:
    video_mode m_video_modes[MAX_VIDEO_MODES];
    int        m_num_video_modes;

public:
    video_mode_mgr();

    void init();

    int get_video_mode_index(int width, int height, int refresh_rate);

    bool is_inited() { return m_num_video_modes > 0; }
};

extern video_mode_mgr g_video_mode_mgr;

//---------------------------------------------------------------------------

class options_config {
public:
    video_mode m_video_mode;
    float m_sfx_volume;
    float m_music_volume;
    float m_mouse_sensitive;
    bool m_mouse_invert_axis;
    bool m_borderless;
public:
    void init_default();
    void load();
    void save();

    options_config();
};

extern options_config g_options_config;

//---------------------------------------------------------------------------

class ui_menu_options {
public:
    bool m_show_settings_menu;
    bool m_show_apply_window;

public:
    ui_menu_options();
    ~ui_menu_options();

    void open() { m_show_settings_menu = true; }

    void draw();
};

extern ui_menu_options g_ui_menu_options;

#endif  // !UI_MENU_H
