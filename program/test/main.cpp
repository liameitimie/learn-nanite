#include "engine.h"
#include "mesh.h"
#include "mesh_simplify.h"
#include <types.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <vec_math.h>

using namespace std;

void print(vec3 v){
    cout<<"("<<v.x<<","<<v.y<<","<<v.z<<")";
}
bool operator!=(vec3 a,vec3 b){
    return a.x!=b.x||a.y!=b.y||a.z!=b.z;
}

int main(){

    // struct data{
    //     int x,y,z;
    //     bool operator<(const data& b) const{
    //         return x<b.x;
    //     }
    // };

    // multiset<data> s;

    // s.insert({1,2,0});
    // s.insert({1,1,1});
    // s.insert({2,0,1});

    // s.erase({1,1,1});

    // for(auto t:s){
    //     cout<<t.x<<' '<<t.y<<' '<<t.z<<endl;
    // }

    // Mesh mesh;
    // // mesh.positions={{0,0,0},{1,0,0},{0,1,0},{1,1,0},{0,0,1},{1,0,1},{0,1,1},{1,1,1}};
    // // mesh.indices={0,1,2,1,2,3, 0,1,4,1,4,5, 0,2,4,2,4,6, 1,3,5,3,5,7, 2,3,6,3,6,7, 4,5,7,4,7,6};
    // mesh.load("asset/spot_triangulated_good.obj");

    // Mesh lod_mesh=simplify(mesh,5000);

    // for(int i=0;i<mesh.positions.size();i++){
    //     for(int j=0;j<i;j++){
    //         vec3 p1=mesh.positions[i];
    //         vec3 p2=mesh.positions[j];
    //         vec3 v=p1-p2;
    //         if(dot(v,v)<1e-6){
    //             if(p1!=p2) cout<<"!!!"<<' '<<i<<' '<<j<<endl;
    //             // cout<<"!!! "<<i<<' '<<j<<endl;
    //             // print(p1);
    //             // cout<<' ';
    //             // print(p2);
    //             // cout<<endl;
    //         }
    //     }
    // }

    Engine engine;
    engine.init();
    engine.run();
    engine.cleanup();
    
    // vec3 a{1,0,0},b{0,1,0};
    // vec3 c=cross(a,b);

    // cout<<c.x<<' '<<c.y<<' '<<c.z<<endl;

    return 0;
}