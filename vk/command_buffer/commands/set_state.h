#pragma once
#include "../../pipeline/viewport.h"
#include <vector>

namespace vk{

void set_viewport(u64 cmd,std::vector<Viewport> viewports);

}