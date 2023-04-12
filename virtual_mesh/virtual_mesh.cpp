#include "virtual_mesh.h"
#include "mesh_simplify.h"
#include <timer.h>
#include <fmt/core.h>
#include <assert.h>

using namespace fmt;

void log_cluster_size(Cluster* clusters,u32 begin,u32 end){
    f32 maxsz=0,minsz=100000,avgsz=0;
    for(u32 i=begin;i<end;i++){
        auto& cluster=clusters[i];
        assert(cluster.verts.size()<256);
        f32 sz=cluster.indexes.size()/3.0;
        if(sz>maxsz) maxsz=sz;
        if(sz<minsz) minsz=sz;
        avgsz+=sz;
    }
    avgsz/=end-begin;
    print("cluster size: max={}, min={}, avg={}\n",maxsz,minsz,avgsz);
}

void log_group_size(ClusterGroup* groups,u32 begin,u32 end){
    f32 maxsz=0,minsz=100000,avgsz=0;
    for(u32 i=begin;i<end;i++){
        f32 sz=groups[i].clusters.size();
        if(sz>maxsz) maxsz=sz;
        if(sz<minsz) minsz=sz;
        avgsz+=sz;
    }
    avgsz/=end-begin;
    print("group size: max={}, min={}, avg={}\n",maxsz,minsz,avgsz);
}

void VirtualMesh::build(Mesh& mesh){
    Timer timer;

    print("\n# begin building virtual mesh\n\n");

    auto& [pos,idx]=mesh;
    timer.reset();
    print("fixup mesh: ");

    //使用简化器并设置大于三角形数目的目标，去除重复点与三角形
    MeshSimplifier simplifier(pos.data(),pos.size(),idx.data(),idx.size());
    simplifier.simplify(idx.size());
    pos.resize(simplifier.remaining_num_vert());
    idx.resize(simplifier.remaining_num_tri()*3);

    print("{} us\n",timer.us());
    print("verts: {}, tris: {}\n\n",pos.size(),idx.size()/3);

    timer.reset();
    print("clustering triangles: ");

    //将三角形分组为三角形簇
    cluster_triangles(pos,idx,clusters);
    print("{} us\n\n",timer.us());

    u32 level_offset=0,mip_level=0;

    print("begin building cluster DAG\n\n");
    while(1){
        print("### level {} ###\n",mip_level);
        print("num clusters: {}\n",clusters.size()-level_offset);
        log_cluster_size(clusters.data(),level_offset,clusters.size());

        u32 num_level_clusters=clusters.size()-level_offset;
        if(num_level_clusters<=1) break;

        u32 prev_cluster_num=clusters.size();
        u32 prev_group_num=cluster_groups.size();

        //将簇分组
        timer.reset();
        print("groupinging clusters: ");
        group_clusters(
            clusters,
            level_offset,
            num_level_clusters,
            cluster_groups,
            mip_level
        );
        print("{} us\n",timer.us());
        print("num groups: {}\n",cluster_groups.size()-prev_group_num);
        log_group_size(cluster_groups.data(),prev_group_num,cluster_groups.size());

        //将组内簇合并并简化，生成上一级簇
        timer.reset();
        print("building parent clusters: ");
        for(u32 i=prev_group_num;i<cluster_groups.size();i++){
            build_parent_clusters(cluster_groups[i],clusters);
        }
        print("{} us\n",timer.us());

        level_offset=prev_cluster_num;
        mip_level++;

        print("\n");
    }
    num_mip_level=mip_level+1;

    print("end building cluster DAG\n");
    print("total clusters: {}\n\n",clusters.size());

    print("# end build virtual mesh\n\n");
}