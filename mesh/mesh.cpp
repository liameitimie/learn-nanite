#include "mesh.h"
#include <iostream>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <assert.h>

// #define TINYOBJLOADER_IMPLEMENTATION
// #include <tiny_obj_loader.h>

using namespace std;

// void Mesh::load(string path){
//     positions.clear();
//     indices.clear();

//     tinyobj::ObjReaderConfig config;
//     config.mtl_search_path="./";
//     tinyobj::ObjReader reader;

//     if(!reader.ParseFromFile(path,config)){
//         if(!reader.Error().empty()){
//             cout<<"error in TinyObjReader: "<<reader.Error();
//         }
//         exit(1);
//     }
//     if(!reader.Warning().empty()){
//         std::cout<<"warning: TinyObjReader: "<<reader.Warning();
//     }
//     auto& attrib = reader.GetAttrib();
//     auto& shapes = reader.GetShapes();

//     for(int i=0;i<attrib.vertices.size();i+=3){
//         auto vx=attrib.vertices[i];
//         auto vy=attrib.vertices[i+1];
//         auto vz=attrib.vertices[i+2];
//         positions.push_back({vx,vy,vz});
//     }

//     for(auto& shape: shapes){
//         i32 offset=0;
//         for(auto cnt: shape.mesh.num_face_vertices){
//             assert(cnt==3);

//             for(int i=0;i<cnt;i++){
//                 auto idx=shape.mesh.indices[offset+i];
//                 indices.push_back(idx.vertex_index);

//                 // auto vx=attrib.vertices[3*idx.vertex_index];
//                 // auto vy=attrib.vertices[3*idx.vertex_index+1];
//                 // auto vz=attrib.vertices[3*idx.vertex_index+2];

//                 // if(idx.normal_index>=0){
//                 //     auto nx=attrib.normals[3*idx.normal_index];
//                 //     auto ny=attrib.normals[3*idx.normal_index+1];
//                 //     auto nz=attrib.normals[3*idx.normal_index+2];
//                 // }
//                 // if(idx.texcoord_index>=0){
//                 //     auto tx=attrib.texcoords[2*idx.texcoord_index];
//                 //     auto ty=attrib.texcoords[2*idx.texcoord_index+1];
//                 // }
//             }
//             offset+=cnt;
//         }
//     }
// }

void Mesh::load(string path){
    Assimp::Importer importer;
    auto scene = importer.ReadFile(path.c_str(), 0);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        cout << "error: failed to load mesh" << endl;
        exit(0);
    }
    auto mesh = scene->mMeshes[0];
    u32 vertex_cnt = mesh->mNumVertices;
    u32 index_cnt = mesh->mNumFaces * 3;

    auto pos = mesh->mVertices;
    auto faces = mesh->mFaces;

    if (!pos||!faces) {
        cout << "invaild mesh" << endl;
        exit(0);
    }
    positions.resize(vertex_cnt);
    for(u32 i=0;i<vertex_cnt;i++){
        positions[i]={
            .x=pos[i].x,
            .y=pos[i].y,
            .z=pos[i].z
        };
    }
    indices.resize(index_cnt);
    for(u32 i=0;i<index_cnt;i+=3){
        assert(faces[i/3].mNumIndices==3);
        indices[i] = faces[i/3].mIndices[0];
        indices[i+1] = faces[i/3].mIndices[1];
        indices[i+2] = faces[i/3].mIndices[2];
    }
}