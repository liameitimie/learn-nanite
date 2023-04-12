#pragma once

#include "cluster.h"
#include <mesh.h>

struct VirtualMesh{
    std::vector<Cluster> clusters;
    std::vector<ClusterGroup> cluster_groups;
    u32 num_mip_level;

    void build(Mesh& mesh);
};