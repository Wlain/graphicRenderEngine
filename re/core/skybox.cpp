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
    m_material = Shader::getSkyboxProcedural()->createMaterial();
    m_material->set("skyColor", Color(0, 0, 1));
    m_material->set("horizonColor", Color(1, 1, 1));
    m_material->set("groundColor", Color(0.31, 0.197, 0.026));
    m_material->set("skyPow", .5f);
    m_material->set("sunIntensity", .15f);
    m_material->set("groundPow", .2f);
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