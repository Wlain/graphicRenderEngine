//
// Created by william on 2022/7/5.
//

#include "basicProject.h"
#include "core/modelImporter.h"
#include "guiCommonDefine.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

/// shadow volume(阴影域)的方式绘制阴影
/// 要构造一个阴影体，先从光源投射一条射线穿过产生阴影的物体中的每个顶点到某个点（通常在无穷远点）。这些射线一起组成一个体；每个该体中的点都在阴影中，所有在外部的物体被该光源照亮。
/// 实际的阴影体如下计算：
/// 找到所有的轮廓边（将前向面和后向面隔开的边）
/// 将所有的轮廓边向远离光源的方向伸展形成一个四边形。
/// 加入前盖和后盖到这些表面上形成一个闭合体（不是必要的，是否采用取决于所采用的实现方法）
/// 链接：https://web.archive.org/web/20100531060920/http://www.gamedev.net/reference/articles/article1873.asp
/*
 * 由Cow提出，可以将阴影投射到任何物体表面
 * 假设视点在所有的shadow volume之外，我们维护一个计数器，其初始值是零。每次当从视点射向目标像素的射线进入到一个shadow volume中时，将计数器+1；而当射线从一个shadow volume中射出时，将计数器-1；这样，我们只需检验当射线到达交点时，计数器是否大于零：如果大于零，则交点位于阴影中；否则不属于阴影区域
 * 第一步：首先，清空模板缓存；
 * 第二步：然后，将整个场景绘制到帧缓存中，这次绘制只使用环境分量和发光分量，并获取相应的颜色信息（在color buffer中）及深度信息（在z-buffer中）；
 * 第三步：关闭颜色缓存的写入和深度检测，绘制所有shadow volume的正面（即射线射入shadow volume时相交的面）：在这个过程中，如果一个像素其深度值小于之前算好的z-buffer中的深度值，那么将这个像素的模板缓存的计数器+1；
 * 第四步：类似于前一步骤，将所有shadow volume的反面绘制一遍，只是这是每发现一个像素其深度值小于之前算好的z-buffer中的深度值，将该像素的模板缓存上的计数器-1；
 * 第五步：再将整个场景根据模板缓存的信息绘制上漫反射分量和高光分量：只有模板缓存是0的像素才绘制，以实现阴影效果。
 * 阴影域算法优点：
 * 1.它可以使用通用的图形学硬件实现，而仅仅需要一个模板缓存；
 * 2.它不是基于图像的方法，shadow volume算法并不会产生由采样和分辨率带来的各种问题，从而可以在任何地方生成正确和清晰的阴影。
 * 阴影域算法缺点：主要体现在性能方面。
 * 由于shadow volume的多边形面片数通常都比较多，且会覆盖住大量的像素，这在很大程度上影响了算法运行和光栅化过程的速度，使得绘制的效率较低。
 */
class StencilExample : public BasicProject
{
public:
    ~StencilExample() override = default;
    void initialize() override
    {
        m_camera.setLookAt(m_eye, m_at, m_up);
        m_camera.setPerspectiveProjection(45.0f, 0.1f, 100.0f);
        std::vector<std::shared_ptr<Material>> materialUnused;
        m_mesh = ModelImporter::importObj("resources/objFiles/suzanne.obj", materialUnused);
        m_plane = Mesh::create().withQuad(2).build();
        m_matStencilWrite = Shader::create()
                                .withSourceFile("embeddedResource/unlit_vert.glsl", Shader::ShaderType::Vertex)
                                .withSourceFile("embeddedResource/unlit_frag.glsl", Shader::ShaderType::Fragment)
                                .withDepthWrite(false)
                                .withColorWrite({ false, false, false, false })
                                .withStencil(Shader::StencilDescriptor{
                                    Shader::StencilFunc ::Always,
                                    1,
                                    1,
                                    Shader::StencilOp::Replace,
                                    Shader::StencilOp::Replace,
                                    Shader::StencilOp::Replace,
                                })
                                .withName("StencilWrite")
                                .build()
                                ->createMaterial();

        m_matStencilTest = Shader::create()
                               .withSourceFile("embeddedResource/unlit_vert.glsl", Shader::ShaderType::Vertex)
                               .withSourceFile("embeddedResource/unlit_frag.glsl", Shader::ShaderType::Fragment)
                               .withStencil(Shader::StencilDescriptor{
                                   Shader::StencilFunc ::LEqual,
                                   1,
                                   1 })
                               .withName("StencilClippedShadow")
                               .build()
                               ->createMaterial();
        m_matStencilTest->setColor({ 0.3f, 0.3f, 0.0f });
        m_material = Shader::getStandardPhong()->createMaterial();
        m_matShadow = Shader::getUnlit()->createMaterial();
        m_matShadow->setColor({ 0.3f, 0.3f, 0.3f });

        m_worldLights = MAKE_UNIQUE(m_worldLights);
        m_worldLights->setAmbientLight(glm::vec3{ 0.05f });
        m_worldLights->addLight(Light::create().withPointLight({ 0.5, 2, 0.5 }).build());
    }

    void render() override
    {
        auto rp = RenderPass::create()
                      .withCamera(m_camera)
                      .withClearColor(true, { 0, 0, 0, 1 })
                      .withClearStencil(true)
                      .withWorldLights(m_worldLights.get())
                      .build();

        auto pos = m_worldLights->getLight(0)->position;
        float bias = 0.01f;
        float y = pos.y - bias - m_shadowPlane;
        // 阴影投影
        glm::mat4 shadow = glm::transpose(glm::mat4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 1 / -y, 0, 0));
        glm::mat4 projectedShadow = glm::translate(pos) * shadow * glm::translate(-pos);
        auto modelMatrix = glm::rotate(m_rotate.x, glm::vec3(1, 0, 0)) * glm::rotate(m_rotate.y, glm::vec3(0, 1, 0));
        if (m_drawShadow)
        {
            if (m_useStencil)
            {
                rp.draw(m_plane, glm::translate(glm::vec3{ 0, m_shadowPlane, 0 }) * glm::rotate(-glm::half_pi<float>(), glm::vec3{ 1, 0, 0 }), m_matStencilWrite);
                rp.draw(m_mesh, projectedShadow * modelMatrix, m_matStencilTest);
            }
            else
            {
                rp.draw(m_mesh, projectedShadow * modelMatrix, m_matShadow);
            }
        }
        // 绘制模型
        rp.draw(m_mesh, modelMatrix, m_material);
        // 绘制平面
        if (m_drawPlane)
        {
            rp.draw(m_plane, glm::translate(glm::vec3{ 0, m_shadowPlane, 0 }) * glm::rotate(-glm::half_pi<float>(), glm::vec3{ 1, 0, 0 }), m_material);
        }
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowContentSize(ImVec2(300, 120));
        ImGui::Begin("Shadow");
        ImGui::DragFloat("Ground height ", &m_shadowPlane, 0.1f);
        ImGui::Checkbox("Draw plane", &m_drawPlane);
        ImGui::Checkbox("Draw shadow", &m_drawShadow);
        ImGui::Checkbox("Use stencil", &m_useStencil);
        ImGui::End();
    }
    void setTitle() override
    {
        m_title = "StencilExample";
    }

private:
    std::shared_ptr<Mesh> m_plane;
    std::shared_ptr<Material> m_matStencilWrite;
    std::shared_ptr<Material> m_matStencilTest;
    std::shared_ptr<Material> m_matShadow;
    glm::vec3 m_eye{ 0, 0, 6.0f };
    glm::vec3 m_at{ 0, 0, 0 };
    glm::vec3 m_up{ 0, 1, 0 };
    float m_shadowPlane = -1.0f;
    bool m_drawPlane = true;
    bool m_drawShadow = true;
    bool m_useStencil = true;
};

void stencilTest()
{
    StencilExample test;
    test.run();
}