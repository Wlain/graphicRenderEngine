//
// Created by william on 2022/6/12.
//

#include "engineTestSimple.h"
#include "guiCommonDefine.h"

class TextureExample : public CommonInterface
{
public:
    using CommonInterface::CommonInterface;
    ~TextureExample() override = default;
    void initialize() override
    {
        m_camera.setOrthographicProjection(3, -2, 2);
        m_filenames = {
            "basn/basn0g01.png", // black & white
            "basn/basn0g02.png", // 2 bit (4 level) grayscale
            "basn/basn0g04.png", // 4 bit (16 level) grayscale
            "basn/basn0g08.png", // 8 bit (256 level) grayscale
            "basn/basn0g16.png", // 16 bit (64k level) grayscale
            "basn/basn2c08.png", // 3x8 bits rgb color
            "basn/basn2c16.png", // 3x16 bits rgb color
            "basn/basn3p01.png", // 1 bit (2 color) paletted
            "basn/basn3p02.png", // 2 bit (4 color) paletted
            "basn/basn3p04.png", // 4 bit (16 color) paletted
            "basn/basn4a08.png", // 8 bit grayscale + 8 bit alpha-channel
            "basn/basn6a08.png", // 3x8 bits rgb color + 8 bit alpha-channel
            "basn/basn6a16.png"  // 3x16 bits rgb color + 16 bit alpha-channel
        };

        for (const auto& s : m_filenames)
        {
            LOG_INFO("Load {}", s);
            m_textures.push_back(Texture::create().withFile(std::string("resources/") + s).build());
        }
        m_mesh = Mesh::create().withCube().build();
        m_material = Shader::create()
                         .withSourceFile("shaders/texture_vert.glsl", Shader::ShaderType::Vertex)
                         .withSourceFile("shaders/texture_frag.glsl", Shader::ShaderType::Fragment)
                         .withBlend(Shader::BlendType::AlphaBlending)
                         .build()
                         ->createMaterial();
    }

    void render() override
    {
        auto renderPass = RenderPass::create()
                              .withCamera(m_camera)
                              .withClearColor(true, { .3, .3, 1, 1 })
                              .build();

        ImGui::ListBox("Texture", &m_selection, m_filenames.data(), m_filenames.size());
        if (m_filenames[m_selection][5] == 'g')
        {
            ImGui::LabelText("png type", "Grayscale");
        }
        if (m_filenames[m_selection][5] == 'c')
        {
            ImGui::LabelText("png type", "Color");
        }
        if (m_filenames[m_selection][5] == 'p')
        {
            ImGui::LabelText("png type", "Palette");
        }
        if (m_filenames[m_selection][5] == 'a')
        {
            ImGui::LabelText("png type", "Alpha");
        }
        const char* colorSpace;
        if (m_textures[m_selection]->getSamplerColorSpace() == Texture::SamplerColorspace::Gamma)
        {
            colorSpace = "Gamma";
        }
        else
        {
            colorSpace = "Linear";
        }
        ImGui::LabelText("Colorspace", "%s", colorSpace);
        ImGui::LabelText("Size", "%d x %d", m_textures[m_selection]->width(), m_textures[m_selection]->height());
        ImGui::LabelText("Transparent", "%s", m_textures[m_selection]->isTransparent() ? "true" : "false");

        m_material->setTexture(m_textures[m_selection]);
        renderPass.draw(m_mesh, glm::mat4(1), m_material);
    }

private:
    std::vector<const char*> m_filenames;
    std::vector<std::shared_ptr<Texture>> m_textures;
    int m_selection = 0;
};

void textureTest()
{
    GLFWRenderer renderer{};
    EngineTestSimple test(renderer);
    auto sceneNodeEffect = std::make_shared<TextureExample>(&renderer);
    auto effect = std::make_shared<EffectManager>();
    effect->insertEffect(sceneNodeEffect);
    test.setEffect(effect);
    test.setTitle("TextureExample");
    test.run();
}