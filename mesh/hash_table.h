#pragma once
#include <types.h>

inline u32 murmur_add(u32 hash,u32 elememt){
	elememt*=0xcc9e2d51;
	elememt=(elememt<<15)|(elememt>>(32-15));
	elememt*=0x1b873593;

	hash^=elememt;
	hash=(hash<<13)|(hash>>(32-13));
	hash=hash*5+0xe6546b64;
	return hash;
}

inline u32 murmur_mix(u32 hash){
	hash^=hash>>16;
	hash*=0x85ebca6b;
	hash^=hash>>13;
	hash*=0xc2b2ae35;
	hash^=hash>>16;
	return hash;
}

inline u32 lower_nearest_2_power(u32 x){
    while(x&(x-1)) x^=(x&-x);
    return x;
}

inline u32 upper_nearest_2_power(u32 x){
    if(x&(x-1)){
        while(x&(x-1)) x^=(x&-x);
        return x==0?1:(x<<1);
    }
    else{
        return x==0?1:(x<<1);
    }
}

class HashTable{
private:
    u32 hash_size;
    u32 hash_mask;
    u32 index_size;
    u32* hash;
    u32* next_index;
    void resize_index(u32 _index_size);
public:
    HashTable(u32 _index_size=0);
    HashTable(u32 _hash_size,u32 _index_size);
    ~HashTable();

    void resize(u32 _index_size);
    void resize(u32 _hash_size,u32 _index_size);

    void free(){
        hash_size=0;
        hash_mask=0;
        index_size=0;
        delete[] hash;
        hash=nullptr;
        delete[] next_index;
        next_index=nullptr;
    }
    void clear();

    void add(u32 key,u32 idx){
        if(idx>=index_size){
            resize_index(upper_nearest_2_power(idx+1));
        }
        key&=hash_mask;
        next_index[idx]=hash[key];
        hash[key]=idx;
    }
    void remove(u32 key,u32 idx){
        if(idx>=index_size) return;
        key&=hash_mask;
        if(hash[key]==idx) hash[key]=next_index[idx];
        else{
            for(u32 i=hash[key];i!=~0u;i=next_index[i]){
                if(next_index[i]==idx){
                    next_index[i]=next_index[idx];
                    break;
                }
            }
        }
    }

    struct Container{
        u32 idx;
        u32* next;
        struct iter{
            u32 idx;
            u32* next;
            void operator++(){idx=next[idx];}
            bool operator!=(const iter& b)const{return idx!=b.idx;}
            u32 operator*(){return idx;}
        };
        iter begin(){return iter{idx,next};}
        iter end(){return iter{~0u,nullptr};}
    };

    Container operator[](u32 key){
        if(hash_size==0||index_size==0) return Container{~0u,nullptr};
        key&=hash_mask;
        return Container{hash[key],next_index};
    }
};