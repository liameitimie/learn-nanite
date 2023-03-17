#pragma once

#include "vk/types.h"

struct Engine{
    int frame_num=0;

    void init();
    void run();
    void cleanup();
};