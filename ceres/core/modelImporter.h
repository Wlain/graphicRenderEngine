//
// Created by william on 2022/5/31.
//

#ifndef SIMPLERENDERENGINE_MODELIMPORTER_H
#define SIMPLERENDERENGINE_MODELIMPORTER_H
#include "material.h"

#include <fileSystem>
#include <memory>
namespace ceres
{
class Mesh;
class ModelImporter
{
public:
    static std::shared_ptr<Mesh> importObj(const std::filesystem::path& path, std::string filename);
    static std::shared_ptr<Mesh> importObj(const std::filesystem::path& path, std::vector<std::shared_ptr<Material>>& outModelMaterials);
    // Load an Obj mesh, materials will be defined in the last parameter.
    // Note that only diffuse color and texture and specular exponent are read from the file
};
} // namespace ceres

#endif // SIMPLERENDERENGINE_MODELIMPORTER_H
