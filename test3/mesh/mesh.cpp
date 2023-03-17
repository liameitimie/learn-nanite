#include "mesh.h"
#include <iostream>
#include <memory>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

using namespace std;

void Mesh::load(string path){
    positions.clear();
    indices.clear();

    tinyobj::ObjReaderConfig config;
    config.mtl_search_path="./";
    tinyobj::ObjReader reader;

    if(!reader.ParseFromFile(path,config)){
        if(!reader.Error().empty()){
            cout<<"error in TinyObjReader: "<<reader.Error();
        }
        exit(1);
    }
    if(!reader.Warning().empty()){
        std::cout<<"warning: TinyObjReader: "<<reader.Warning();
    }
    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();

    for(int i=0;i<attrib.vertices.size();i+=3){
        auto vx=attrib.vertices[i];
        auto vy=attrib.vertices[i+1];
        auto vz=attrib.vertices[i+2];
        positions.push_back({vx,vy,vz});
    }

    for(auto& shape: shapes){
        i32 offset=0;
        for(auto cnt: shape.mesh.num_face_vertices){
            assert(cnt==3);

            for(int i=0;i<cnt;i++){
                auto idx=shape.mesh.indices[offset+i];
                indices.push_back(idx.vertex_index);

                // auto vx=attrib.vertices[3*idx.vertex_index];
                // auto vy=attrib.vertices[3*idx.vertex_index+1];
                // auto vz=attrib.vertices[3*idx.vertex_index+2];

                // if(idx.normal_index>=0){
                //     auto nx=attrib.normals[3*idx.normal_index];
                //     auto ny=attrib.normals[3*idx.normal_index+1];
                //     auto nz=attrib.normals[3*idx.normal_index+2];
                // }
                // if(idx.texcoord_index>=0){
                //     auto tx=attrib.texcoords[2*idx.texcoord_index];
                //     auto ty=attrib.texcoords[2*idx.texcoord_index+1];
                // }
            }
            offset+=cnt;
        }
    }
}