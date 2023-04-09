#pragma once
#include <types.h>

class BitArray{
    u32* bits;
public:
    BitArray(){bits=nullptr;}
    BitArray(u32 size);
    ~BitArray(){free();}
    void resize(u32 size);
    void free(){
        if(bits) delete[] bits;
    }
    void set_false(u32 idx){
        u32 x=idx>>5;
        u32 y=idx&31;
        bits[x]&=~(1<<y);
    }
    void set_true(u32 idx){
        u32 x=idx>>5;
        u32 y=idx&31;
        bits[x]|=(1<<y);
    }
    bool operator[](u32 idx){
        u32 x=idx>>5;
        u32 y=idx&31;
        return (bool)(bits[x]>>y&1);
    }
};