#include <iostream>
#include <vector>
#include <partitioner.h>
// #include <metis.h>

using namespace std;

// idx_t n,m;
// vector<idx_t> xadj;
// vector<idx_t> adjncy; //压缩图表示
// vector<idx_t> adjwgt; //边权重
// vector<idx_t> part;

int n,m;

// int a[]={
// 1,2,10,
// 2,3,10,
// 3,6,10,
// 6,9,10,
// 9,8,10,
// 8,7,10,
// 7,4,10,
// 4,1,10,
// 2,5,3,
// 5,8,2,
// 5,6,4,
// 4,5,1
// };

int main(){
    Graph g;
    cin>>n>>m;
    // n=9,m=12;
    g.init(n);
    for(int i=0;i<m;i++){
        int u,v,w;
        cin>>u>>v>>w;
        // u=a[i*3+0],v=a[i*3+1],w=a[i*3+2];
        u--,v--;
        g.add_edge(u,v,w);
        g.add_edge(v,u,w);
    }
    Partitioner p;
    p.partition(g,2,4);

    for(auto[l,r]:p.ranges){
        cout<<l<<' '<<r<<endl;
        for(int i=l;i<r;i++){
            cout<<p.node_id[i]<<' ';
        }
        cout<<endl;
    }
    for(u32 x:p.sort_to) cout<<x<<' ';cout<<endl;
    // cin>>n>>m;
    // for(int i=0;i<n;i++){
    //     int k;
    //     cin>>k;
    //     xadj.push_back(adjncy.size());
    //     for(int j=0;j<k;j++){
    //         int t,w;
    //         cin>>t>>w;
    //         adjncy.push_back(t-1);
    //         adjwgt.push_back(w);
    //     }
    // }
    // xadj.push_back(adjncy.size());

    // part.resize(n);

    // real_t part_weight[]={
    //     5.f/7.f,
    //     2.f/7.f
    // };
    // idx_t nw=1,npart=2,ncut=0;
    // int res=METIS_PartGraphRecursive(
    //     &n,
    //     &nw,xadj.data(),
    //     adjncy.data(),
    //     nullptr, //vert weights
    //     nullptr, //vert size
    //     adjwgt.data(),
    //     &npart,
    //     part_weight, //partition weight
    //     nullptr, 
    //     nullptr, //options
    //     &ncut,
    //     part.data()
    // );

    // if(res==METIS_OK){
    //     for(auto i:part){
    //         cout<<i<<' ';
    //     }
    // }
    // cout<<endl;
    return 0;
}

/*
7 11
3  5 1 3 2 2 1
3  1 1 3 2 4 1
4  5 3 4 2 2 2 1 2
4  2 1 3 2 6 2 7 5
3  1 1 3 3 6 2
3  5 2 4 2 7 6
2  6 6 4 5

9 12
1 2 10
2 3 10
3 6 10
6 9 10
9 8 10
8 7 10
7 4 10
4 1 10
2 5 3
5 8 2
5 6 4
4 5 1
*/