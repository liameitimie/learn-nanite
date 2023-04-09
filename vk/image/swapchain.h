#pragma once
#include "image.h"
#include "image_view.h"
#include "../error.h"
#include <result.h>
#include <vector>
#include "../commit.h"
#include <option.h>

namespace vk{

void acquire_next_image(Option<Semaphore> semaphore,Option<Fence> fence,u32& image_idx);

}