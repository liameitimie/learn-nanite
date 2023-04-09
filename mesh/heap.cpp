#include "heap.h"
#include <memory>
#include <assert.h>

Heap::Heap(){
    heap=nullptr,keys=nullptr,heap_indexes=nullptr;
    heap_size=0,num_index=0;
}

Heap::Heap(u32 _num_index){
    heap_size=0;
    num_index=_num_index;
    heap=new u32[num_index];
    keys=new f32[num_index];
    heap_indexes=new u32[num_index];
    memset(heap_indexes,0xff,num_index*sizeof(u32));
}

void Heap::resize(u32 _num_index){
    free();
    heap_size=0;
    num_index=_num_index;
    heap=new u32[num_index];
    keys=new f32[num_index];
    heap_indexes=new u32[num_index];
    memset(heap_indexes,0xff,num_index*sizeof(u32));
}

void Heap::push_up(u32 i){
    u32 idx=heap[i];
    u32 fa=(i-1)>>1;
    while(i>0&&keys[idx]<keys[heap[fa]]){
        heap[i]=heap[fa];
        heap_indexes[heap[i]]=i;
        i=fa,fa=(i-1)>>1;
    }
    heap[i]=idx;
    heap_indexes[heap[i]]=i;
}

void Heap::push_down(u32 i){
    u32 idx=heap[i];
    u32 ls=(i<<1)+1;
    u32 rs=ls+1;
    while(ls<heap_size){
        u32 t=ls;
        if(rs<heap_size&&keys[heap[rs]]<keys[heap[ls]])
            t=rs;
        if(keys[heap[t]]<keys[idx]){
            heap[i]=heap[t];
            heap_indexes[heap[i]]=i;
            i=t,ls=(i<<1)+1,rs=ls+1;
        }
        else break;
    }
    heap[i]=idx;
    heap_indexes[heap[i]]=i;
}

void Heap::clear(){
    heap_size=0;
    memset(heap_indexes,0xff,num_index*sizeof(u32));
}

u32 Heap::top(){
    assert(heap_size>0);
    return heap[0];
}

void Heap::pop(){
    assert(heap_size>0);

    u32 idx=heap[0];
    heap[0]=heap[--heap_size];
    heap_indexes[heap[0]]=0;
    heap_indexes[idx]=~0u;
    push_down(0);
}

void Heap::add(f32 key,u32 idx){
    assert(!is_present(idx));

    u32 i=heap_size++;
    heap[i]=idx;
    keys[idx]=key;
    heap_indexes[idx]=i;
    push_up(i);
}

void Heap::update(f32 key,u32 idx){
    keys[idx]=key;
    u32 i=heap_indexes[idx];
    if(i>0&&key<keys[heap[(i-1)>>1]]) push_up(i);
    else push_down(i);
}

void Heap::remove(u32 idx){
    //if(!is_present(idx)) return;
    assert(is_present(idx));

    f32 key=keys[idx];
    u32 i=heap_indexes[idx];

    if(i==heap_size-1){
        --heap_size;
        heap_indexes[idx]=~0u;
        return;
    }

    heap[i]=heap[--heap_size];
    heap_indexes[heap[i]]=i;
    heap_indexes[idx]=~0u;
    if(key<keys[heap[i]]) push_down(i);
    else push_up(i);
}

f32 Heap::get_key(u32 idx){
    return keys[idx];
}