#pragma once

#include <chrono>
#include "types.h"

class Timer{
public:
    Timer(){
        reset();
    }
    void reset(){
        start=std::chrono::high_resolution_clock::now();
    }
    u64 us(){ //当前时钟减去开始时钟的count
        return duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
    }
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
};
