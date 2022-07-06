//
// Created by william on 2022/6/20.
//
#include "basicProject.h"
#include "guiCommonDefine.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

using namespace re;
const int BOX_GRID_DIM = 30;
class Benchmark64kExample : public BasicProject
{
public:
    using BasicProject::BasicProject;
    ~Benchmark64kExample() override = default;
    void initialize() override
    {
        m_camera.setPerspectiveProjection(90.0f, 0.1f, 100.0f);
        m_material = Shader::getUnlit()->createMaterial();
        m_material->setTexture(Texture::create().withFile("resources/block.jpg").withGenerateMipmaps(true).build());
        m_mesh = Mesh::create().withCube().build();
        int boxI = 0;
        int offset = (int)-floor(BOX_GRID_DIM / 2);
        for (int i = 0; i < BOX_GRID_DIM; ++i)
        {
            for (int j = 0; j < BOX_GRID_DIM; ++j)
            {
                for (int k = 0; k < BOX_GRID_DIM; ++k)
                {
                    m_box[i][j][k] = {
                        (float)(boxI / M_PI),
                        glm::rotate((float)(boxI / M_PI), glm::vec3(1, 1, 1)),
                        glm::translate(glm::vec3(i + offset, j + offset, k + offset))
                    };
                    boxI++;
                }
            }
        }
    }

    void render() override
    {
        m_eyeRotation += 0.002;
        m_eyePosition[0] = (float)(sin(m_eyeRotation) * m_eyeRadius);
        m_eyePosition[2] = (float)(cos(m_eyeRotation) * m_eyeRadius);
        m_camera.setLookAt(m_eyePosition, { 0, 0, 0 }, { 0, 1, 0 });
        auto renderPass = RenderPass::create()
                              .withCamera(m_camera)
                              .withClearColor(true, { 0, 0, 0, 1 })
                              .build();
        for (int i = 0; i < m_gridSize; ++i)
        {
            for (int j = 0; j < m_gridSize; ++j)
            {
                for (int k = 0; k < m_gridSize; ++k)
                {
                    auto& boxRef = m_box[i][j][k];
                    // update rotation
                    boxRef.rotationMatrix = glm::rotate(boxRef.rotationMatrix, 0.02f, glm::vec3(1, 1, 1));
                    m_modelMatrix[i][j][k] = boxRef.translationMatrix * boxRef.rotationMatrix;
                    renderPass.draw(m_mesh, m_modelMatrix[i][j][k], m_material);
                }
            }
        }
        m_inspector.update();
        m_inspector.gui();
        ImGui::SliderInt("Grid size", &m_gridSize, 1, BOX_GRID_DIM);
    }
    void setTitle() override
    {
        m_title = "Benchmark64kExample";
    }

public:
    struct Box
    {
        float rotate;
        glm::mat4 rotationMatrix;
        glm::mat4 translationMatrix;
    };

private:
    int m_gridSize = BOX_GRID_DIM / 2;
    float m_eyeRadius = 30;
    float m_eyeRotation = 0;
    glm::vec3 m_eyePosition = { 0, 3, 0 };
    Box m_box[BOX_GRID_DIM][BOX_GRID_DIM][BOX_GRID_DIM]{};
    glm::mat4 m_modelMatrix[BOX_GRID_DIM][BOX_GRID_DIM][BOX_GRID_DIM]{};
};

void benchmark64kTest()
{
    Benchmark64kExample test;
    test.run();
}