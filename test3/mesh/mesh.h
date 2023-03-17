#pragma once
#include <types.h>
#include <vector>
#include <string>
#include <pipeline/vertex_input.h>
#include <vector>

struct Mesh{
    std::vector<vec3> positions;
    std::vector<u32> indices;

    static auto vertex_desc(){
        std::vector member{
            vk::VertexMemberInfo{
                .offset=0,
                .format=vk::Format::R32G32B32_SFLOAT
            }
        };
        return std::vector{vk::VertexBufferDescription{
            .member=member,
            .stride=sizeof(vec3),
            .input_rate=vk::VertexInputRate::Vertex
        }};
    }

    void load(std::string path);

    std::vector<vec3> flatten(){
        std::vector<vec3> vertices;
        for(auto idx: indices){
            vertices.push_back(positions[idx]);
        }
        return vertices;
    }
};