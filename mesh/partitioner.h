#pragma once

#include <vector>
#include <map>
#include <types.h>

struct Graph{
    std::vector<std::map<u32,i32>> g;

    void init(u32 n){
        g.resize(n);
    }
    void add_node(){
        g.push_back({});
    }
    void add_edge(u32 from,u32 to,i32 cost){
        g[from][to]=cost;
    }
    void increase_edge_cost(u32 from,u32 to,i32 i_cost){
        g[from][to]+=i_cost;
    }
};

struct MetisGraph;

class Partitioner{
    u32 bisect_graph(MetisGraph* graph_data,MetisGraph* child_graphs[2],u32 start,u32 end);
    void recursive_bisect_graph(MetisGraph* graph_data,u32 start,u32 end);
public:
    void init(u32 num_node);
    void partition(const Graph& graph,u32 min_part_size,u32 max_part_size);

    std::vector<u32> node_id; //将节点按划分编号排序
    std::vector<std::pair<u32,u32>> ranges; //分块的连续范围，范围内是相同划分
    std::vector<u32> sort_to;
    u32 min_part_size;
    u32 max_part_size;
};