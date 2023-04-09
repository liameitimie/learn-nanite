#include <iostream>
#include <mesh.h>
#include <cluster.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

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


int main(){
    Timer timer;
    Mesh mesh;

    timer.reset();
    mesh.load("../asset/spot_triangulated_good.obj");
    cout<<"load mesh: "<<timer.us()<<" us"<<endl;

    const auto&[pos,idx]=mesh;

    vector<Cluster> clusters;

    timer.reset();
    cluster_triangles(pos,idx,clusters);
    cout<<"cluster triangles: "<<timer.us()<<" us"<<endl;

    int id=0;
    for(auto& cluster:clusters){
        cout<<endl;
        cout<<"cluster "<<id<<endl;
        cout<<"num vertexs: "<<cluster.verts.size()<<endl;
        cout<<"num edge: "<<cluster.indexes.size()<<endl;
        cout<<"num triangle: "<<cluster.indexes.size()/3<<endl;
        cout<<"num ext edge: "<<cluster.external_edges.size()<<endl;
        id++;
    }

    return 0;
}

/*

*/