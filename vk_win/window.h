#pragma once
#include <vec_types.h>

namespace vk_win{

struct WindowContext;

struct Window{
    u64 handle;
    u32 width;
    u32 height;
    u64 surface;
    WindowContext* context;
    
    static Window create(u32 width,u32 height,const char* name);

    void build_vk_surface(u64 instance);

    bool should_close();
    void poll_events();

    bool is_key_down(char c);
    bool is_key_begin_press(char c);
    bool is_key_begin_release(char c);

    dvec2 get_cursor_pos();

    void set_cursor_disabled();
    void set_cursor_normal();
};

}