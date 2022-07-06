//
// Created by cwb on 2022/6/21.
//

#include "skybox.h"

#include "material.h"
#include "mesh.h"
namespace re
{
Skybox::Skybox()
{
    m_skyboxMesh = Mesh::create().withSphere(16, 32, -1).build();
    m_material = Shader::getSkybox()->createMaterial();
}

std::shared_ptr<Skybox> Skybox::create()
{
    return std::shared_ptr<Skybox>(new Skybox());
}

const std::shared_ptr<Material>& Skybox::getMaterial() const
{
    return m_material;
}

void Skybox::setMaterial(std::shared_ptr<Material> material)
{
    m_material = std::move(material);
}
} // namespace re