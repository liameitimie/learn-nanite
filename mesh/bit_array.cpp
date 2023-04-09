#include "bit_array.h"
#include <memory>

BitArray::BitArray(u32 size){
    bits=new u32[(size+31)/32];
    memset(bits,0,(size+31)/32*sizeof(u32));
}

void BitArray::resize(u32 size){
    free();
    bits=new u32[(size+31)/32];
    memset(bits,0,(size+31)/32*sizeof(u32));
}