#include <iostream>
#include <chrono>
#include <time.h>
#include <vcruntime.h>
#include <vector>
#include <rusty-iter.h>
#include <vec_types.h>
#include <assert.h>
#include <mesh.h>
#include <random>
#include <heap.h>
#include <mesh_simplify.h>
#include <hash_table.h>

using namespace std;
using namespace std::chrono;
using namespace rusty;

class Timer{
public:
    Timer(){
        reset();
    }
    void reset(){
        start = high_resolution_clock::now();
    }
    long long us(){
    //当前时钟减去开始时钟的count
        return duration_cast<microseconds>(high_resolution_clock::now() - start).count();
    }
private:
    time_point<high_resolution_clock> start;
};

u32 cycle3(u32 i){
    u32 imod3=i%3;
    return i-imod3+(1<<imod3&3);
}

int main(){
    for(int i=0;i<10;i++){
        cout<<i<<' '<<cycle3(i)<<endl;
    }
    // Heap h(10);
    // vector<u32> v(10);

    // for(int i=0;i<10;i++){
    //     v[i]=rand();
    //     h.add(v[i],i);
    // }
    // for(u32 x:v) cout<<x<<' ';cout<<endl;

    // h.update(1,3);
    // h.remove(6);

    // int i=0;
    // while(!h.empty()){
    //     cout<<i<<' '<<v[h.top()]<<endl;
    //     h.pop();
    //     i++;
    // }

    // BitArray v(100);
    // v.set_true(1);
    // v.set_true(6);
    // v.set_true(11);
    // v.set_true(2);
    // v.set_true(6);
    // //v.set_false(11);
    // v.set_true(64);
    // //v.set_true(65);
    // v.set_true(63);
    // //v.set_true(62);
    // //v.set_false(2);

    // for(int i=0;i<100;i++){
    //     cout<<i<<' '<<v[i]<<endl;
    // }

    // Timer timer;

    // timer.reset();
    // Mesh mesh;
    // mesh.load("../asset/spot_triangulated_good.obj");
    // cout<<timer.us()<<endl;
    // auto&[pos,idx]=mesh;

    // MeshSimplifier simplifier(pos.data(),pos.size(),idx.data(),idx.size());
    // simplifier.simplify(3000);

    // pos.resize(simplifier.remaining_num_vert());
    // idx.resize(simplifier.remaining_num_tri()*3);

    // HashTable ht(pos.size());
    // cout<<pos.size()<<endl;

    // auto hash=[](vec3 v)->u32{
    //     union {f32 f;u32 u;} x,y,z;
    //     x.f=v.x,y.f=v.y,z.f=v.z;
    //     return murmur_mix(murmur_add(murmur_add(x.u,y.u),z.u));
    // };

    // timer.reset();
    // int i=0;
    // for(vec3 p:pos){
    //     ht.add(hash(p),i);
    //     i++;
    // }
    // cout<<timer.us()<<endl;

    // int t=0;

    // timer.reset();
    // for(vec3 p:pos){
    //     for(u32 i=ht.first(::hash(p));ht.is_valid(i);i=ht.next(i)){
    //         if(p==pos[i]){
    //             t++;
    //         }
    //     }
    // }
    // cout<<timer.us()<<' '<<t<<endl;

    // timer.reset();
    // for(vec3 p:pos){
    //     for(auto i:ht[hash(p)]){
    //         if(p==pos[i]){
    //             t++;
    //         }
    //     }
    // }
    // cout<<timer.us()<<' '<<t<<endl;

    

    
    return 0;
}