//
// Created by cwb on 2022/7/7.
//
#include "basicProject.h"
#include "core/skybox.h"
#include "guiCommonDefine.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

#define DAMPING 0.01f           // 阻尼：胡克定律 f = kd
#define DELTA_TIME2 0.5f * 0.5f // delta t 的平方
#define CONSTRAINT_ITERATIONS 7 // 约束

// Verlet积分会使用上一个时刻的状态来推下一个时刻的状态

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

    // f = m * a
    void timeStep()
    {
        if (m_movable)
        {
            glm::vec3 temp = m_pos;
            // X-t+1 = X-t + (X-t - X-t-1) * (1.0f - damping) * a deltaTime * deltaTime;
            m_pos = m_pos + (m_pos - m_lastPos) * (1.0f - DAMPING) + m_acceleration * DELTA_TIME2;
            m_lastPos = temp;
            // 此时加速度已经转化成速度，需要重置
            m_acceleration = glm::vec3(0, 0, 0);
        }
    }

    [[nodiscard]] const glm::vec3& getPos() const
    {
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

    [[nodiscard]] const glm::vec3& getNormal() const { return m_accumulatedNormal; }
    void resetNormal() { m_accumulatedNormal = glm::vec3(0, 0, 0); }

private:
    bool m_movable{ true };                            // 表示当前质点是否可以移动
    float m_mass{ 1.0f };                              // 质量
    glm::vec3 m_pos{};                                 // 当前位置
    glm::vec3 m_lastPos{};                             // 上一帧的位置，用于verlet数值积分,Verlet 方法是一种求解离散时间上的积分方法
    glm::vec3 m_acceleration{ 0.0f, 0.0f, 0.0f };      // 加速度
    glm::vec3 m_accumulatedNormal{ 0.0f, 0.0f, 0.0f }; // 法线累积值
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
        glm::vec3 vec = m_mass2->getPos() - m_mass1->getPos();
        float currentDistance = length(vec);
        glm::vec3 correctionVector = vec * (1.0f - m_restDistance / currentDistance); // p1移动到静止距离到 p 的偏移向量
        glm::vec3 correctionVectorHalf = correctionVector * 0.5f;                     // 把它的长度减半，这样我们就可以同时移动 p1和 p2。
        m_mass1->offsetPos(correctionVectorHalf);                                     // 纠正向量半指向p1到p2，所以长度应该移动p1满足约束所需长度的一半
        m_mass2->offsetPos(-correctionVectorHalf);                                    // 移动 p2校正向量半的负方向，因为它指向 p2到 p1，而不是 p1到 p2
    }

private:
    float m_restDistance = 0.0f; // 质点之间的距离
    MassSpringSystem *m_mass1 = nullptr, *m_mass2 = nullptr;
};

class Cloth
{
public:
    Cloth(float width, float height, int massCountWidth, int massCountHeight, bool showMesh = false) :
        m_massCountWidth(massCountWidth), m_massCountHeight(massCountHeight), m_showMesh(showMesh)
    {
        m_massSprings.resize(massCountWidth * massCountHeight);

        // 创建所有质点
        for (int x = 0; x < massCountWidth; ++x)
        {
            for (int y = 0; y < massCountHeight; ++y)
            {
                glm::vec3 pos = { width * (x / (float)massCountWidth), -height * (y / (float)massCountHeight), 0 };
                m_massSprings[y * massCountWidth + x] = MassSpringSystem(pos);
            }
        }

        // 水平和竖直方向用绳索连接所有质点
        for (int x = 0; x < massCountWidth; ++x)
        {
            for (int y = 0; y < massCountHeight; ++y)
            {
                if (x < massCountWidth - 1) makeRope(getMassSpring(x, y), getMassSpring(x + 1, y));
                if (y < massCountHeight - 1) makeRope(getMassSpring(x, y), getMassSpring(x, y + 1));
                if (x < massCountWidth - 1 && y < massCountHeight - 1) makeRope(getMassSpring(x, y), getMassSpring(x + 1, y + 1));
                if (x < massCountWidth - 1 && y < massCountHeight - 1) makeRope(getMassSpring(x + 1, y), getMassSpring(x, y + 1));
            }
        }

        // 相邻（距离为2）约束所有质点
        for (int x = 0; x < massCountWidth; ++x)
        {
            for (int y = 0; y < massCountHeight; ++y)
            {
                if (x < massCountWidth - 2) makeRope(getMassSpring(x, y), getMassSpring(x + 2, y));
                if (y < massCountHeight - 2) makeRope(getMassSpring(x, y), getMassSpring(x, y + 2));
                if (x < massCountWidth - 2 && y < massCountHeight - 2) makeRope(getMassSpring(x, y), getMassSpring(x + 2, y + 2));
                if (x < massCountWidth - 2 && y < massCountHeight - 2) makeRope(getMassSpring(x + 2, y), getMassSpring(x, y + 2));
            }
        }

        // 固定左上角和右上角的两个顶点
        for (int i = 0; i <= 3; ++i)
        {
            getMassSpring(0 + i, 0)->makeUnmovable();
            getMassSpring(massCountWidth - 1 - i, 0)->makeUnmovable();
        }

        m_mesh = Mesh::create()
                     .withName("Cloth mesh")
                     .withPositions(getPositions())
                     .withNormals(getNormals())
                     .withUvs(getUVs())
                     .withIndices(createIndices())
                     .withUsage(Mesh::BufferUsage::DynamicDraw)
                     .withMeshTopology(m_showMesh ? Mesh::Topology::LineStrip : Mesh::Topology::TriangleStrip)
                     .build();

        m_material = Shader::create()
                         .withSourceFile("shaders/standard_blinn_phong_vert.glsl", Shader::ShaderType::Vertex)
                         .withSourceFile("shaders/standard_blinn_phong_frag.glsl", Shader::ShaderType::Fragment)
                         .withName("Standard Blinn Phong")
                         .withCullFace(Shader::CullFace::None)
                         .build()
                         ->createMaterial();
        m_material->setColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        m_material->setMetallicRoughness({ 0.5f, 0.5f });
        m_material->setTexture(Texture::create().withFile("resources/block.jpg").build());
    }

    std::vector<glm::vec3> getPositions()
    {
        std::vector<glm::vec3> res;
        for (int y = 0; y < m_massCountHeight; y++)
        {
            for (int x = 0; x < m_massCountWidth; x++)
            {
                res.push_back(getMassSpring(x, y)->getPos());
            }
        }
        return res;
    }

    std::vector<glm::vec3> getNormals()
    {
        std::vector<glm::vec3> res;
        for (int y = 0; y < m_massCountHeight; y++)
        {
            for (int x = 0; x < m_massCountWidth; x++)
            {
                res.push_back(getMassSpring(x, y)->getNormal());
            }
        }
        return res;
    }

    [[nodiscard]] std::vector<glm::vec4> getUVs() const
    {
        std::vector<glm::vec4> res;
        for (int y = 0; y < m_massCountHeight; y++)
        {
            for (int x = 0; x < m_massCountWidth; x++)
            {
                glm::vec4 uv(x / (m_massCountHeight - 1.0f), y / (m_massCountWidth - 1.0f), 0.0f, 0.0f);
                res.push_back(uv);
            }
        }
        return res;
    }

    [[nodiscard]] std::vector<uint16_t> createIndices() const
    {
        std::vector<uint16_t> indices;

        for (int j = 0; j < m_massCountHeight - 1; j++)
        {
            int index = 0;
            if (j > 0)
            {
                indices.push_back(static_cast<uint16_t>(j * m_massCountWidth)); // make degenerate
            }
            for (int i = 0; i <= m_massCountWidth - 1; i++)
            {
                index = j * m_massCountWidth + i;
                indices.push_back(static_cast<uint16_t>(index));
                indices.push_back(static_cast<uint16_t>(index + m_massCountWidth));
            }
            if (j + 1 < m_massCountHeight - 1)
            {
                indices.push_back(static_cast<uint32_t>(index + m_massCountWidth)); // make degenerate
            }
        }
        return indices;
    }

    /* 绘制三角形面片
     (x,y)   *--* (x+1,y)
        | /|
        |/ |
     (x,y+1) *--* (x+1,y+1)
    */
    void render(RenderPass& rp)
    {
        // 重置法线
        for (auto& mass : m_massSprings)
        {
            mass.resetNormal();
        }

        // 通过将每个粒子所属的所有三角形法线相加，创建平滑的每个粒子法线（取平均）
        for (int x = 0; x < m_massCountWidth - 1; ++x)
        {
            for (int y = 0; y < m_massCountHeight - 1; ++y)
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

        m_mesh->update()
            .withPositions(getPositions())
            .withNormals(getNormals())
            .build();

        rp.draw(m_mesh, glm::mat4(1.0f), m_material);
    }

    // 对所有质点调用timeStep()
    void timeStep()
    {
        for (int i = 0; i < CONSTRAINT_ITERATIONS; ++i)
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

    // 重力(或任何其他任意矢量)添加到所有质点
    void addForce(const glm::vec3& direction)
    {
        for (auto& mass : m_massSprings)
        {
            mass.addForce(direction); // add the forces to each particle
        }
    }

    // 给所有质点增加风力，是为每个三角形增加的，因为最终的力与从风向 */看到的三角形面积成正比
    void windForce(const glm::vec3& direction)
    {
        for (int x = 0; x < m_massCountWidth - 1; ++x)
        {
            for (int y = 0; y < m_massCountHeight - 1; ++y)
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
    [[nodiscard]] int getMassIndex(int x, int y) const { return y * m_massCountWidth + x; }
    MassSpringSystem* getMassSpring(int x, int y) { return &m_massSprings[getMassIndex(x, y)]; }
    void makeRope(MassSpringSystem* p1, MassSpringSystem* p2) { m_rope.emplace_back(p1, p2); }

    // 计算三角形面片的法向量
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
    std::vector<MassSpringSystem> m_massSprings;
    std::vector<Rope> m_rope;
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Material> m_material;
    int m_massCountWidth;  // 横向质点数
    int m_massCountHeight; // 纵向质点数
    bool m_showMesh = false;
};

class ClothSimulationExample : public BasicProject
{
public:
    ~ClothSimulationExample() override = default;
    void initialize() override
    {
        m_cloth = std::make_shared<Cloth>(30, 20, m_massCountWidth, m_massCountHeight);
        m_camera.setLookAt({ 0, 0, 3 }, { 0, 0, 0 }, { 0, 1, 0 });
        m_camera.setPerspectiveProjection(80, 0.1, 100);
        m_sphereMaterial = Shader::getStandardPBR()->createMaterial({ { "S_TWO_SIDED", "true" } });
        m_sphereMaterial->setColor({ 1.0f, 1.0f, 1.0f, 1.0f });
        m_sphereMaterial->setMetallicRoughness({ 0.5f, 0.5f });
        m_sphere = Mesh::create().withSphere().build();
        m_worldLights = MAKE_UNIQUE(m_worldLights);
        m_worldLights->addLight(Light::create().withPointLight({ -10, -10, 10 }).withColor({ 1, 0, 0 }).build());
        m_worldLights->addLight(Light::create().withPointLight({ -30, -30, 10 }).withColor({ 0, 1, 0 }).build());
        auto tex = Texture::create()
                       .withFileCubeMap("resources/skybox/park3Med/px.jpg", Texture::CubeMapSide::PositiveX)
                       .withFileCubeMap("resources/skybox/park3Med/nx.jpg", Texture::CubeMapSide::NegativeX)
                       .withFileCubeMap("resources/skybox/park3Med/py.jpg", Texture::CubeMapSide::PositiveY)
                       .withFileCubeMap("resources/skybox/park3Med/ny.jpg", Texture::CubeMapSide::NegativeY)
                       .withFileCubeMap("resources/skybox/park3Med/pz.jpg", Texture::CubeMapSide::PositiveZ)
                       .withFileCubeMap("resources/skybox/park3Med/nz.jpg", Texture::CubeMapSide::NegativeZ)
                       .withWrapUV(Texture::Wrap::ClampToEdge)
                       .build();
        m_skybox = Skybox::create();
        m_skybox->getMaterial()->setTexture(tex);
    }
    void render() override
    {
        auto renderPass = RenderPass::create()
                              .withCamera(m_camera)
                              .withWorldLights(m_worldLights.get())
                              .withSkybox(m_skybox)
                              .withName("Frame")
                              .build();

        // setup light
        m_cloth->render(renderPass);
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
        bool updated = ImGui::Checkbox("ShowMesh", &m_showMesh);
        updated |= ImGui::DragInt("Particles width", &m_massCountWidth, 5, 5, 100);
        updated |= ImGui::DragInt("MassSpring height", &m_massCountHeight, 5, 5, 100);
        ImGui::LabelText("MassSpring count", "%i", m_massCountWidth * m_massCountHeight);
        ImGui::DragFloat3("eye", &m_eye.x);
        ImGui::DragFloat3("at", &m_at.x);
        ImGui::DragFloat3("up", &m_up.x);
        m_camera.setLookAt(m_eye, m_at, m_up);
        if (updated)
        {
            m_cloth = std::make_shared<Cloth>(30, 20, m_massCountWidth, m_massCountHeight, m_showMesh);
        }
        ImGui::End();
        m_inspector.update();
        m_inspector.gui();
    }
    void update(float deltaTime) override
    {
        BasicProject::update(deltaTime);
        m_ballTime++;
        m_ballPos.z = cos(m_ballTime / 50.0f) * CONSTRAINT_ITERATIONS;
        m_cloth->addForce(m_gravity * DELTA_TIME2);
        m_cloth->windForce(m_wind * DELTA_TIME2);
        m_cloth->timeStep();
        m_cloth->ballCollision(m_ballPos, m_ballRadius + m_ballColliderEpsilon);
    }
    void setTitle() override
    {
        m_title = "ClothSimulationExample";
    }

private:
    inline static float s_damping = 0.01f;
    inline static int s_constraintIterations = 7;

private:
    std::shared_ptr<Mesh> m_sphere;
    std::shared_ptr<Material> m_sphereMaterial;
    std::shared_ptr<Cloth> m_cloth;
    std::shared_ptr<Skybox> m_skybox;
    glm::vec3 m_gravity = { 0, -0.2, 0 }; // 重力
    glm::vec3 m_wind{ 0.5, 0, 0.2 };      // 风力
    glm::vec3 m_ballPos{ 15, -7, 0 };     // 球心
    glm::vec3 m_eye{ -4, 0, -30.0f };
    glm::vec3 m_at{ 0, 0, 0 };
    glm::vec3 m_up{ 0, 1, 0 };
    float m_ballColliderEpsilon = .1; // 球与布料碰撞的最小距离
    float m_ballRadius = 5;           // 球半径
    int m_massCountWidth{ 55 };       // 横向质点数
    int m_massCountHeight{ 45 };      // 纵向质点数
    float m_ballTime{ 0 };            // 用于计算下面球的 z 位置的计数器
    bool m_showMesh = false;
};

void clothSimulationTest()
{
    ClothSimulationExample test;
    test.run();
}