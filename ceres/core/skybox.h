//
// Created by cwb on 2022/6/21.
//

#ifndef GRAPHICRENDERENGINE_SKYBOX_H
#define GRAPHICRENDERENGINE_SKYBOX_H
#include <memory>
namespace ceres
{
class Material;
class Mesh;
class Skybox
{
public:
    static std::shared_ptr<Skybox> create();
    [[nodiscard]] const std::shared_ptr<Material>& getMaterial() const;
    void setMaterial(std::shared_ptr<Material> material);

private:
    Skybox();

private:
    std::shared_ptr<Material> m_material;
    std::shared_ptr<Mesh> m_skyboxMesh;
    friend class RenderPass;
};
} // namespace ceres
#endif // GRAPHICRENDERENGINE_SKYBOX_H
