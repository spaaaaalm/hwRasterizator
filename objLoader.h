#pragma once
#include <vector>
#include <string>
#include <iostream>
#include "mathTypes/math.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "mathTypes/tiny_obj_loader.h"

struct Mesh {
    std::vector<Vec3f> positions;
    std::vector<Vec3f> normals;
    std::vector<Vec2f> uvs; //текстурные координаты
    std::vector<int> indices; // треугольники: каждые 3 числа
};

inline bool loadOBJ(const std::string &filename, Mesh &mesh)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    bool ret = tinyobj::LoadObj(
        &attrib, &shapes, &materials, &warn, &err,
        filename.c_str(), nullptr, true, true
    );

    if (!warn.empty()) std::cout << "WARN: " << warn << "\n";
    if (!err.empty())  std::cout << "ERR: " << err << "\n";
    if (!ret) return false;


    mesh.positions.clear();
    mesh.normals.clear();
    mesh.uvs.clear();
    mesh.indices.clear();

    for (auto &shape : shapes) {
        for (size_t f = 0; f < shape.mesh.indices.size(); f++) {
            auto idx = shape.mesh.indices[f]; //разворот индексов в плоские массивы

            // vertex
            mesh.positions.push_back({
                attrib.vertices[3 * idx.vertex_index + 0], //x,y,z
                attrib.vertices[3 * idx.vertex_index + 1],
                attrib.vertices[3 * idx.vertex_index + 2]
            });

            // normal
            if (idx.normal_index >= 0) {
                mesh.normals.push_back({
                    attrib.normals[3 * idx.normal_index + 0],
                    attrib.normals[3 * idx.normal_index + 1],
                    attrib.normals[3 * idx.normal_index + 2]
                });
            } else {
                mesh.normals.push_back({0,0,1});
            }

            // uv
            if (idx.texcoord_index >= 0) {
                mesh.uvs.push_back({
                    attrib.texcoords[2 * idx.texcoord_index + 0],
                    attrib.texcoords[2 * idx.texcoord_index + 1]
                });
            } else {
                mesh.uvs.push_back({0,0});
            }

            mesh.indices.push_back((int)mesh.indices.size());
        }
    }

    return true;
}
