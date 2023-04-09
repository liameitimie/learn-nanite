#pragma once
#include <types.h>

class Heap{
    u32 heap_size;
    u32 num_index;
    u32* heap;
    f32* keys;
    u32* heap_indexes;

    void push_up(u32 i);
    void push_down(u32 i);
public:
    Heap();
    Heap(u32 _num_index);
    ~Heap(){free();}

    void free(){
        heap_size=0,num_index=0;
        delete[] heap;
        delete[] keys;
        delete[] heap_indexes;
        heap=nullptr,keys=nullptr,heap_indexes=nullptr;
    }
    void resize(u32 _num_index);
    
    f32 get_key(u32 idx);
    void clear();
    bool empty(){return heap_size==0;}
    bool is_present(u32 idx){return heap_indexes[idx]!=~0u;}
    u32 top();
    void pop();
    void add(f32 key,u32 idx);
    void update(f32 key,u32 idx);
    void remove(u32 idx);
};