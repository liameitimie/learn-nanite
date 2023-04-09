#pragma once

#include <window.h>

struct Engine{
    int frame_num=0;

    vk_win::Window window;

    void init();
    void run();
    void cleanup();
};