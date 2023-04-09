#pragma once

#include <types.h>

inline u32 cycle3(u32 i){
    u32 imod3=i%3;
    return i-imod3+((1<<imod3)&3);
}
inline u32 cycle3(u32 i,u32 ofs){
    return i-i%3+(i+ofs)%3;
}
