//
// Created by cwb on 2022/7/7.
//
#include "basicProject.h"
#include "core/skybox.h"
#include "guiCommonDefine.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

// 质点弹簧类
class MassSpringSystem
{
public:
    explicit MassSpringSystem(glm::vec3 pos) :
        m_pos(pos), m_lastPos(pos)
    {}
    MassSpringSystem() = default;

    void addForce(glm::vec3 f)
    {
        m_acceleration += f / m_mass;
    }

    void timeStep()
    {
        // 隐式欧拉求解
        if (m_movable)
        {
            glm::vec3 temp = m_pos;
            m_pos = m_pos + (m_pos - m_lastPos) * (1.0f - s_damping) + m_acceleration * s_timeStepSize2;
            m_lastPos = temp;
            m_acceleration = glm::vec3(0, 0, 0);
        }
    }

    const glm::vec3& getPos() const {
        LOG_ERROR("pos:{}, {}, {}", m_pos.x, m_pos.y, m_pos.z);
        return m_pos;
    }

    void resetAcceleration() { m_acceleration = glm::vec3(0, 0, 0); }

    void offsetPos(const glm::vec3 v)
    {
        if (m_movable)
            m_pos += v;
    }

    void makeUnmovable() { m_movable = false; }

    void addToNormal(glm::vec3 normal)
    {
        m_accumulatedNormal += normalize(normal);
    }

    const glm::vec3& getNormal() const { return m_accumulatedNormal; }

    void resetNormal() { m_accumulatedNormal = glm::vec3(0, 0, 0); }

private:
    // 物理常量
    inline static float s_damping = 0.01f;
    inline static int s_constraintIterations = 7;
    inline static float s_timeStepSize2 = 0.5f * 0.5f;

private:
    bool m_movable{ true };                // 表示当前质点是否可以移动
    float m_mass{ 1.0f };                  // 质量
    glm::vec3 m_pos;                       // 当前位置
    glm::vec3 m_lastPos;                   // 上一帧的位置，用于verlet数值积分
    glm::vec3 m_acceleration{ 0.0f };      // 加速度
    glm::vec3 m_accumulatedNormal{ 0.0f }; // 法线累积值
};

/// 绳索类
class Rope
{
public:
    Rope(MassSpringSystem* m1, MassSpringSystem* m2) :
        m_mass1(m1), m_mass2(m2)
    {
        auto vec = m_mass1->getPos() - m_mass2->getPos();
        m_restDistance = length(vec);
    }

    void satisfyConstraint()
    {
        glm::vec3 vec = m_mass1->getPos() - m_mass2->getPos();
        float currentDistance = length(vec);
        glm::vec3 correctionVector = vec * (1 - m_restDistance / currentDistance); // p1移动到静止距离到 p 的偏移向量
        glm::vec3 correctionVectorHalf = correctionVector * 0.5f;                  // 把它的长度减半，这样我们就可以同时移动 p1和 p2。
        m_mass1->offsetPos(correctionVectorHalf);                                  // 纠正向量半指向p1到p2，所以长度应该移动p1满足约束所需长度的一半
        m_mass2->offsetPos(-correctionVectorHalf);                                 // 移动 p2校正向量半的负方向，因为它指向 p2到 p1，而不是 p1到 p2
    }

private:
    float m_restDistance; // 质点之间的距离
    MassSpringSystem *m_mass1, *m_mass2;
};

class Cloth
{
public:
    Cloth(float width, float height, int massCountH, int massCountV) :
        m_massCountH(massCountH), m_massCountV(massCountV)
    {
        m_massSprings.resize(massCountH * massCountV);

        // 创建所有质点
        for (int x = 0; x < massCountH; x++)
        {
            for (int y = 0; y < massCountV; y++)
            {
                glm::vec3 pos = { width * (x / (float)massCountH), -height * (y / (float)massCountV), 0 };
                m_massSprings[y * massCountH + x] = MassSpringSystem(pos);
            }
        }

        // 水平和竖直方向用绳索连接所有质点
        for (int x = 0; x < massCountH; x++)
        {
            for (int y = 0; y < massCountV; y++)
            {
                if (x < massCountH - 1) makeRope(getMassSpring(x, y), getMassSpring(x + 1, y));
                if (y < massCountV - 1) makeRope(getMassSpring(x, y), getMassSpring(x, y + 1));
                if (x < massCountH - 1 && y < massCountV - 1) makeRope(getMassSpring(x, y), getMassSpring(x + 1, y + 1));
                if (x < massCountH - 1 && y < massCountV - 1) makeRope(getMassSpring(x + 1, y), getMassSpring(x, y + 1));
            }
        }

        // 【左上，右下】，【左下，右上】方向用绳索连接所有质点
        for (int x = 0; x < massCountH; x++)
        {
            for (int y = 0; y < massCountV; y++)
            {
                if (x < massCountH - 2) makeRope(getMassSpring(x, y), getMassSpring(x + 2, y));
                if (y < massCountV - 2) makeRope(getMassSpring(x, y), getMassSpring(x, y + 2));
                if (x < massCountH - 2 && y < massCountV - 2) makeRope(getMassSpring(x, y), getMassSpring(x + 2, y + 2));
                if (x < massCountH - 2 && y < massCountV - 2) makeRope(getMassSpring(x + 2, y), getMassSpring(x, y + 2));
            }
        }

        // 固定左上角和右上角的两个顶点
        for (int i = 1; i <= 2; i++)
        {
            getMassSpring(0 + i, 0)->offsetPos({ 0.5, 0.0, 0.0 });
            getMassSpring(0 + i, 0)->makeUnmovable();

            getMassSpring(0 + i, 0)->offsetPos({ -0.5, 0.0, 0.0 });
            getMassSpring(massCountH - 1 - i, 0)->makeUnmovable();
        }

        m_mesh = Mesh::create()
                     .withName("Cloth mesh")
                     .withPositions(getPositions())
                     .withNormals(getNormals())
                     .withUvs(getUVs())
                     .withIndices(createIndices())
                     .withMeshTopology(Mesh::Topology::TriangleStrip)
                     .build();

        m_material = Shader::getStandardPBR()->createMaterial({ { "S_TWO_SIDED", "true" } });
        m_material->setColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        m_material->setMetallicRoughness({ .5f, .5f });

        auto color1 = glm::u8vec4(255, 255, 255, 255);
        auto color2 = glm::u8vec4(153, 51, 51, 255);
        auto texture = buildClothTexture(color1, color2, massCountH - 1);
        m_material->setTexture(texture);
    }

    // 构建布料纹理
    std::shared_ptr<Texture> buildClothTexture(glm::u8vec4 color1, glm::u8vec4 color2, int width)
    {
        std::vector<glm::u8vec4> textureData;
        for (int i = 0; i < width; i++)
        {
            if (i % 2 == 0)
            {
                textureData.push_back(color1);
            }
            else
            {
                textureData.push_back(color2);
            }
        }
        auto dataPtr = glm::value_ptr(textureData[0]);
        return Texture::create().withRGBAData(reinterpret_cast<const char*>(dataPtr), width, 1).withGenerateMipmaps(false).withFilterSampling(false).build();
    }

    std::vector<glm::vec3> getPositions()
    {
        std::vector<glm::vec3> res;
        for (int y = 0; y < m_massCountV; y++)
        {
            for (int x = 0; x < m_massCountH; x++)
            {
                res.push_back(getMassSpring(x, y)->getPos());
            }
        }
        return res;
    }

    std::vector<glm::vec3> getNormals()
    {
        std::vector<glm::vec3> res;
        for (int y = 0; y < m_massCountV; y++)
        {
            for (int x = 0; x < m_massCountH; x++)
            {
                res.push_back(getMassSpring(x, y)->getNormal());
            }
        }
        return res;
    }

    std::vector<glm::vec4> getUVs()
    {
        std::vector<glm::vec4> res;
        for (int y = 0; y < m_massCountV; y++)
        {
            for (int x = 0; x < m_massCountH; x++)
            {
                glm::vec4 uv(x / (m_massCountV - 1.0f), y / (m_massCountH - 1.0f), 0.0f, 0.0f);
                res.push_back(uv);
            }
        }
        return res;
    }

    std::vector<uint16_t> createIndices()
    {
        std::vector<uint16_t> indices;

        for (int j = 0; j < m_massCountV - 1; j++)
        {
            int index = 0;
            if (j > 0)
            {
                indices.push_back(static_cast<uint16_t>(j * m_massCountH)); // make degenerate
            }
            for (int i = 0; i <= m_massCountH - 1; i++)
            {
                index = j * m_massCountH + i;
                indices.push_back(static_cast<uint16_t>(index));
                indices.push_back(static_cast<uint16_t>(index + m_massCountH));
            }
            if (j + 1 < m_massCountV - 1)
            {
                indices.push_back(static_cast<uint32_t>(index + m_massCountH)); // make degenerate
            }
        }
        return indices;
    }

    /* 将布料画成平滑的阴影(并根据柱子着色) OpenGL 三角网格从 display()方法调用布料看起来是由四个三角形的粒子组成的网格如下:

     (x,y)   *--* (x+1,y)
        | /|
        |/ |
     (x,y+1) *--* (x+1,y+1)
    */
    void drawShaded(RenderPass& rp)
    {
        // 重置法线
        for (auto& mass : m_massSprings)
        {
            mass.resetNormal();
        }

        // 通过将每个粒子所属的所有三角形法线相加，创建平滑的每个粒子法线（取平均）
        for (int x = 0; x < m_massCountH - 1; x++)
        {
            for (int y = 0; y < m_massCountV - 1; y++)
            {
                auto normal = calcTriangleNormal(getMassSpring(x + 1, y), getMassSpring(x, y), getMassSpring(x, y + 1));
                getMassSpring(x + 1, y)->addToNormal(normal);
                getMassSpring(x, y)->addToNormal(normal);
                getMassSpring(x, y + 1)->addToNormal(normal);

                normal = calcTriangleNormal(getMassSpring(x + 1, y + 1), getMassSpring(x + 1, y), getMassSpring(x, y + 1));
                getMassSpring(x + 1, y + 1)->addToNormal(normal);
                getMassSpring(x + 1, y)->addToNormal(normal);
                getMassSpring(x, y + 1)->addToNormal(normal);
            }
        }

        // update mesh data
//        m_mesh->update()
//            .withPositions(getPositions())
//            .withNormals(getNormals())
//            .build();

        rp.draw(m_mesh, glm::mat4(1.0f), m_material);
    }

    // 对所有质点调用timeStep()
    void timeStep()
    {
        for (int i = 0; i < s_timeStepSize2; i++)
        {
            for (auto& rope : m_rope)
            {
                rope.satisfyConstraint();
            }
        }

        for (auto& mass : m_massSprings)
        {
            mass.timeStep();
        }
    }

    // 重力(或任何其他任意矢量)添加到所有粒子
    void addForce(const glm::vec3& direction)
    {
        for (auto& mass : m_massSprings)
        {
            mass.addForce(direction); // add the forces to each particle
        }
    }

    // 用于给所有粒子增加风力，是为每个三角形增加的，因为最终的力与从风向 */看到的三角形面积成正比
    void windForce(const glm::vec3& direction)
    {
        for (int x = 0; x < m_massCountH - 1; ++x)
        {
            for (int y = 0; y < m_massCountV - 1; ++y)
            {
                addWindForcesForTriangle(getMassSpring(x + 1, y), getMassSpring(x, y), getMassSpring(x, y + 1), direction);
                addWindForcesForTriangle(getMassSpring(x + 1, y + 1), getMassSpring(x + 1, y), getMassSpring(x, y + 1), direction);
            }
        }
    }

    // 用于检测和解决布料与球的碰撞。这是基于一个非常简单的方案，其中每个粒子的位置是简单的比较球和校正。如果球与粒子之间的网格距离相比足够小，球体就可以“滑过”
    void ballCollision(const glm::vec3& center, float radius)
    {
        for (auto& mass : m_massSprings)
        {
            auto v = mass.getPos() - center;
            float l = length(v);
            if (length(v) < radius) // if the mass is inside the ball
            {
                mass.offsetPos(normalize(v) * (radius - l)); //把质点投射到球的表面
            }
        }
    }

private:
    int getParticleIndex(int x, int y) const { return y * m_massCountH + x; }
    MassSpringSystem* getMassSpring(int x, int y) { return &m_massSprings[getParticleIndex(x, y)]; }
    void makeRope(MassSpringSystem* p1, MassSpringSystem* p2) { m_rope.emplace_back(p1, p2); }

    // 法向量的大小等于由 m1，p2和 p3定义的平行四边形的面积
    glm::vec3 calcTriangleNormal(MassSpringSystem* m1, MassSpringSystem* m2, MassSpringSystem* m3)
    {
        glm::vec3 pos1 = m1->getPos();
        glm::vec3 pos2 = m2->getPos();
        glm::vec3 pos3 = m3->getPos();
        glm::vec3 v1 = pos2 - pos1;
        glm::vec3 v2 = pos3 - pos1;
        return cross(v1, v2);
    }

    // 用于计算单个三角形的风力
    void addWindForcesForTriangle(MassSpringSystem* m1, MassSpringSystem* m2, MassSpringSystem* m3, const glm::vec3 direction)
    {
        glm::vec3 normal = calcTriangleNormal(m1, m2, m3);
        glm::vec3 d = normalize(normal);
        glm::vec3 force = normal * (dot(d, direction));
        m1->addForce(force);
        m2->addForce(force);
        m3->addForce(force);
    }

private:
    inline static float s_timeStepSize2 = 0.5f * 0.5f;

private:
    struct Vertex
    {
        glm::vec3 position;
        glm::vec2 uv;
        glm::vec3 normal;
    };
    std::vector<MassSpringSystem> m_massSprings;
    std::vector<Rope> m_rope;
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Material> m_material;
    int m_massCountH; // 横向弹簧数
    int m_massCountV; // 纵向弹簧数
};

class ClothSimulationExample : public BasicProject
{
public:
    ~ClothSimulationExample() override = default;
    void initialize() override
    {
        m_cloth = std::make_shared<Cloth>(6, 8, m_massCountH, m_massCountV);
        m_camera.setLookAt({ 0, 0, 20 }, { 0, 0, 0 }, { 0, 1, 0 });
        m_camera.setPerspectiveProjection(80, 0.1, 100);
        m_sphereMaterial = Shader::getStandardPBR()->createMaterial();
        m_sphereMaterial->setColor({ 0.0f, 1.0f, 0.0f, 1.0f });
        m_sphereMaterial->setMetallicRoughness({ .5f, .5f });
        m_sphere = Mesh::create().withSphere().build();
        m_worldLights = MAKE_UNIQUE(m_worldLights);
        m_worldLights->setAmbientLight({ 0.05, 0.05, 0.05 });
        m_worldLights->addLight(Light::create().withDirectionalLight({ 1, 1, 1 }).withColor({ 1, 1, 1 }).build());
//        auto tex = Texture::create()
//                       .withFileCubeMap("resources/skybox/park3Med/px.jpg", Texture::CubeMapSide::PositiveX)
//                       .withFileCubeMap("resources/skybox/park3Med/nx.jpg", Texture::CubeMapSide::NegativeX)
//                       .withFileCubeMap("resources/skybox/park3Med/py.jpg", Texture::CubeMapSide::PositiveY)
//                       .withFileCubeMap("resources/skybox/park3Med/ny.jpg", Texture::CubeMapSide::NegativeY)
//                       .withFileCubeMap("resources/skybox/park3Med/pz.jpg", Texture::CubeMapSide::PositiveZ)
//                       .withFileCubeMap("resources/skybox/park3Med/nz.jpg", Texture::CubeMapSide::NegativeZ)
//                       .withWrapUV(Texture::Wrap::ClampToEdge)
//                       .build();
//        m_skybox = Skybox::create();
//        m_skybox->getMaterial()->setTexture(tex);
    }
    void render() override
    {
        auto renderPass = RenderPass::create()
                              .withCamera(m_camera)
                              .withWorldLights(m_worldLights.get())
//                              .withSkybox(m_skybox)
                              .withName("Frame")
                              .build();

        // setup light
        m_cloth->drawShaded(renderPass);
        // draw solid sphere
        renderPass.draw(m_sphere, glm::translate(m_ballPos) * glm::scale(glm::vec3(m_ballRadius, m_ballRadius, m_ballColliderEpsilon)), m_sphereMaterial);
        auto size = Renderer::s_instance->getWindowSize();
        ImVec2 imSize(250, 220.0f);
        ImVec2 imPos(size.x - 250, 0);
        ImGui::SetNextWindowSize(imSize); // imgui window size should have same width as SDL window size
        ImGui::SetNextWindowPos(imPos);
        ImGui::Begin("Cloth simulation settings", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
        // create window without title
        ImGui::DragInt("Constraint iterations", &s_constraintIterations, 1, 1, 30);
        ImGui::DragFloat("Damping", &s_damping, 0.05, 0, 1);
        ImGui::DragFloat3("Gravity", &m_gravity.x);
        ImGui::DragFloat3("Wind", &m_wind.x);
        ImGui::DragFloat("Ball size", &m_ballRadius, 1.5, 0.25, 5);
        bool updated = ImGui::DragInt("Particles width", &m_massCountH, 5, 5, 100);
        updated |= ImGui::DragInt("Particles height", &m_massCountV, 5, 5, 100);
        ImGui::LabelText("Particle count", "%i", m_massCountH * m_massCountV);
        if (updated)
        {
            m_cloth = std::make_shared<Cloth>(14, 10, m_massCountH, m_massCountV);
        }
        ImGui::End();
        m_inspector.update();
        m_inspector.gui();
    }
    void update(float deltaTime) override
    {
        BasicProject::update(deltaTime);
        m_ballTime++;
        m_ballPos.y = cos(m_ballTime / 50.0f) * 7;
        m_cloth->addForce(m_gravity * s_timeStepSize2);                          // add gravity each frame, pointing down
        m_cloth->windForce(m_wind * s_timeStepSize2);                            // generate some wind each frame
        m_cloth->timeStep();                                                     // calculate the particle positions of the next frame
        m_cloth->ballCollision(m_ballPos, m_ballRadius + m_ballColliderEpsilon); // resolve collision with the ball
    }
    void setTitle() override
    {
        m_title = "ClothSimulationExample";
    }

private:
    inline static float s_timeStepSize2 = 0.5f * 0.5f;
    inline static float s_damping = 0.01f;
    inline static int s_constraintIterations = 7;

private:
    std::shared_ptr<Mesh> m_sphere;
    std::shared_ptr<Material> m_sphereMaterial;
    std::shared_ptr<Cloth> m_cloth;
    std::shared_ptr<Skybox> m_skybox;
    glm::vec3 m_gravity = { 0, -0.2, 0 }; // 重力
    glm::vec3 m_wind{ 0.5, 0, 0.2 };      // 风力
    glm::vec3 m_ballPos{ 7, -5, 0 };      // 球心
    float m_ballColliderEpsilon = .1;     // 球与布料碰撞的最小距离
    float m_ballRadius = 2;               // 球半径
    int m_massCountH{ 10 };               // 横向弹簧数
    int m_massCountV{ 10 };               // 纵向弹簧数
    float m_ballTime{ 0 };                // 用于计算下面球的 z 位置的计数器
};

void clothSimulationTest()
{
    ClothSimulationExample test;
    test.run();
}