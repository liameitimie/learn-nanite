#pragma once

#include <vec_types.h>
#include <vector>
#include "bounds.h"

struct Cluster{
    static const u32 cluster_size=128;

    std::vector<vec3> verts;
    std::vector<u32> indexes;
    std::vector<u32> external_edges;

    Bounds box_bounds;
    Sphere sphere_bounds;
    Sphere lod_bounds;
    f32 lod_error;
    u32 mip_level;
    u32 group_id;
};

struct ClusterGroup{
    static const u32 min_group_size=8;
    static const u32 max_group_size=16;

    Sphere bounds;
    Sphere lod_bounds;
    f32 min_lod_error;
    f32 max_parent_lod_error;
    u32 mip_level;
    std::vector<u32> clusters; //对cluster数组的下标
    std::vector<std::pair<u32,u32>> external_edges; //first: cluster id, second: edge id
};

void cluster_triangles(
    const std::vector<vec3>& verts,
    const std::vector<u32>& indexes,
    std::vector<Cluster>& clusters
);

void group_clusters(
    std::vector<Cluster>& clusters,
    u32 offset,
    u32 num_cluster,
    std::vector<ClusterGroup>& cluster_groups
);

void build_parent_clusters(
    ClusterGroup& cluster_group,
    std::vector<Cluster>& clusters
);