#include "hash_table.h"
#include <assert.h>
#include <memory.h>

HashTable::HashTable(u32 _index_size){
    hash=nullptr,next_index=nullptr;
    resize(_index_size);
}

HashTable::HashTable(u32 _hash_size,u32 _index_size){
    hash=nullptr,next_index=nullptr;
    resize(_hash_size,_index_size);
}

HashTable::~HashTable(){
    free();
}

void HashTable::resize(u32 _index_size){
    resize(lower_nearest_2_power(_index_size),_index_size);
}

void HashTable::resize(u32 _hash_size,u32 _index_size){
    free();
    assert((_hash_size&(_hash_size-1))==0);

    hash_size=_hash_size;
    hash_mask=hash_size-1;
    index_size=_index_size;
    hash=new u32[hash_size];
    next_index=new u32[index_size];
    memset(hash,0xff,hash_size*4);
}

void HashTable::resize_index(u32 _index_size){
    u32* indexs=new u32[_index_size];
    memcpy(indexs,next_index,sizeof(u32)*index_size);
    delete[] next_index;
    next_index=indexs;
    index_size=_index_size;
}

void HashTable::clear(){
    memset(hash,0xff,hash_size*4); 
}