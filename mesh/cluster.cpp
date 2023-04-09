#include "cluster.h"
#include "mesh_simplify.h"
#include "partitioner.h"
#include "hash_table.h"
#include "mesh_util.h"
#include <vec_math.h>
#include "bounds.h"
#include <assert.h>
#include <span>
#include <unordered_map>

using namespace std;

inline u32 hash(vec3 v){
    union {f32 f;u32 u;} x,y,z;
    x.f=(v.x==0.f?0:v.x);
    y.f=(v.y==0.f?0:v.y);
    z.f=(v.z==0.f?0:v.z);
    return murmur_mix(murmur_add(murmur_add(x.u,y.u),z.u));
}

inline u32 hash(pair<vec3,vec3> e){
    u32 h0=::hash(e.first);
    u32 h1=::hash(e.second);
    return murmur_mix(murmur_add(h0,h1));
}

//将原来的数位以2个0分隔：10111->1000001001001，用于生成莫顿码 
inline u32 expand_bits(u32 v){
	v=(v*0x00010001u)&0xFF0000FFu;
    v=(v*0x00000101u)&0x0F00F00Fu;
    v=(v*0x00000011u)&0xC30C30C3u;
    v=(v*0x00000005u)&0x49249249u;
    return v;
}
//莫顿码，要求 0<=x,y,z<=1
u32 morton3D(vec3 p){
    u32 x=p.x*1023,y=p.y*1023,z=p.z*1023;
	x=expand_bits(x);
	y=expand_bits(y);
	z=expand_bits(z);
	return (x<<2)|(y<<1)|(z<<1);
}

//边哈希，找到共享顶点且相反的边，代表两三角形相邻
void build_adjacency_edge_link(
    const vector<vec3>& verts,
    const vector<u32>& indexes,
    Graph& edge_link
){
    HashTable edge_ht(indexes.size());
    edge_link.init(indexes.size());

    for(u32 i=0;i<indexes.size();i++){
        vec3 p0=verts[indexes[i]];
        vec3 p1=verts[indexes[cycle3(i)]];
        edge_ht.add(::hash({p0,p1}),i);

        for(u32 j:edge_ht[::hash({p1,p0})]){
            if(p1==verts[indexes[j]]&&p0==verts[indexes[cycle3(j)]]){
                edge_link.increase_edge_cost(i,j,1);
                edge_link.increase_edge_cost(j,i,1);
            }
        }
    }
}

// todo: 莫顿码排序后在不同联通块间连边，确保网格联通
// void build_locality_link_tris(
//     const vector<vec3>& verts,
//     const vector<u32>& indexes,
//     Graph& graph
// ){
//     Bounds bounds={verts[0],verts[0]};
//     for(vec3 p:verts){
//         bounds=bounds+p;
//     }
//     vec3 extent=bounds.pmax-bounds.pmin;
//     f32 max_len=max(max(extent.x,extent.y),extent.z);

//     for(u32 i=0;i<indexes.size()/3;i++){
//         vec3 p0=verts[indexes[i*3]];
//         vec3 p1=verts[indexes[i*3]+1];
//         vec3 p2=verts[indexes[i*3]+2];
//         vec3 center=(p0+p1+p2)*(1.0f/3.0f);
//         center=(center-bounds.pmin)*(1/max_len);

//         u32 morton=morton3D(center);

//     }
// }

// 根据边的邻接构建三角形的邻接图，边权为1，当需要加入local时需要adjacency边权足够大
void build_adjacency_graph(
    const Graph& edge_link,
    Graph& graph
){
    graph.init(edge_link.g.size()/3);
    u32 u=0;
    for(const auto& mp:edge_link.g){
        for(auto[v,w]:mp){
            graph.increase_edge_cost(u/3,v/3,1);
        }
        u++;
    }
}

void cluster_triangles(
    const vector<vec3>& verts,
    const vector<u32>& indexes,
    vector<Cluster>& clusters
){
    Graph edge_link,graph;
    build_adjacency_edge_link(verts,indexes,edge_link);
    build_adjacency_graph(edge_link,graph);

    Partitioner partitioner;
    partitioner.partition(graph,Cluster::cluster_size-4,Cluster::cluster_size);

    // todo: 包围盒计算
    // 根据划分结果构建clusters
    for(auto[l,r]:partitioner.ranges){
        clusters.push_back({});
        Cluster& cluster=clusters.back();
        unordered_map<u32,u32> mp;
        for(u32 i=l;i<r;i++){
            u32 t_idx=partitioner.node_id[i];
            for(u32 k=0;k<3;k++){
                u32 e_idx=t_idx*3+k;
                u32 v_idx=indexes[e_idx];
                if(mp.find(v_idx)==mp.end()){ //重映射顶点下标
                    mp[v_idx]=cluster.verts.size();
                    cluster.verts.push_back(verts[v_idx]);
                }
                bool is_external=false;
                for(auto[adj_edge,_]:edge_link.g[e_idx]){
                    u32 adj_tri=partitioner.sort_to[adj_edge/3];
                    if(adj_tri<l||adj_tri>=r){ //出点在不同划分说明是边界
                        is_external=true;
                        break;
                    }
                }
                if(is_external){
                    cluster.external_edges.push_back(cluster.indexes.size());
                }
                cluster.indexes.push_back(mp[v_idx]);
            }
        }
    }
}

void build_clusters_edge_link(
    span<const Cluster> clusters,
    const vector<pair<u32,u32>>& ext_edges,
    Graph& edge_link
){
    HashTable edge_ht(ext_edges.size());
    edge_link.init(ext_edges.size());

    u32 i=0;
    for(auto[c_id,e_id]:ext_edges){
        auto& pos=clusters[c_id].verts;
        auto& idx=clusters[c_id].indexes;
        vec3 p0=pos[idx[e_id]];
        vec3 p1=pos[idx[cycle3(e_id)]];
        edge_ht.add(::hash({p0,p1}),i);
        for(u32 j:edge_ht[::hash({p1,p0})]){
            auto [c_id1,e_id1]=ext_edges[j];
            auto& pos1=clusters[c_id1].verts;
            auto& idx1=clusters[c_id1].indexes;
            
            if(pos1[idx1[e_id1]]==p1&&pos1[idx1[cycle3(e_id1)]]==p0){
                edge_link.increase_edge_cost(i,j,1);
                edge_link.increase_edge_cost(j,i,1);
            }
        }
        i++;
    }
}

void build_clusters_graph(
    const Graph& edge_link,
    const vector<u32>& mp,
    u32 num_cluster,
    Graph& graph
){
    graph.init(num_cluster);
    u32 u=0;
    for(const auto& emp:edge_link.g){
        for(auto [v,w]:emp){
            graph.increase_edge_cost(mp[u],mp[v],1);
        }
        u++;
    }
}

void group_clusters(
    vector<Cluster>& clusters,
    u32 offset,
    u32 num_cluster,
    vector<ClusterGroup>& cluster_groups
){
    span<const Cluster> clusters_view(clusters.begin()+offset,num_cluster);

    //取出每个cluster的边界，并建立边id到簇id的映射
    vector<u32> mp; //edge_id to cluster_id
    vector<u32> mp1; //cluster_id to first_edge_id
    vector<pair<u32,u32>> ext_edges;
    u32 i=0;
    for(auto& cluster:clusters_view){
        mp1.push_back(mp.size());
        for(u32 e:cluster.external_edges){
            ext_edges.push_back({i,e});
            mp.push_back(i);
        }
        i++;
    }
    Graph edge_link,graph;
    build_clusters_edge_link(clusters_view,ext_edges,edge_link);
    build_clusters_graph(edge_link,mp,num_cluster,graph);

    Partitioner partitioner;
    partitioner.partition(graph,ClusterGroup::min_group_size,ClusterGroup::max_group_size);

    //todo: 包围盒
    for(auto [l,r]:partitioner.ranges){
        cluster_groups.push_back({});
        auto& group=cluster_groups.back();
        for(u32 i=l;i<r;i++){
            u32 c_id=partitioner.node_id[i];
            clusters[c_id+offset].group_id=cluster_groups.size()-1;
            group.clusters.push_back(c_id+offset);
            for(u32 e_idx=mp1[c_id];e_idx<mp.size()&&mp[e_idx]==c_id;e_idx++){
                bool is_external=false; 
                for(auto [adj_e,_]:edge_link.g[e_idx]){
                    u32 adj_cl=partitioner.sort_to[mp[adj_e]];
                    if(adj_cl<l||adj_cl>=r){
                        is_external=true;
                        break;
                    }
                }
                if(is_external){
                    u32 e=ext_edges[e_idx].second;
                    group.external_edges.push_back({c_id+offset,e});
                }
            }
        }
    }
}

void build_parent_clusters(
    ClusterGroup& cluster_group,
    std::vector<Cluster>& clusters
){
    vector<vec3> pos;
    vector<u32> idx;
    for(u32 c:cluster_group.clusters){
        auto& cluster=clusters[c];
        for(vec3 p:cluster.verts) pos.push_back(p);
        for(u32 i:cluster.indexes) idx.push_back(i);
    }

    MeshSimplifier simplifier(pos.data(),pos.size(),idx.data(),idx.size());
    for(auto [c,e]:cluster_group.external_edges){
        auto& p=clusters[c].verts;
        auto& i=clusters[c].indexes;
        simplifier.lock_position(p[i[e]]);
        simplifier.lock_position(p[i[cycle3(e)]]);
    }
    simplifier.simplify(Cluster::cluster_size/2*cluster_group.clusters.size());
}