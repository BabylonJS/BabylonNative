#include <gtest/gtest.h>

#include <Babylon/Plugins/ShaderCache.h>
#include <Babylon/Plugins/ShaderCacheInternal.h>

#include <gsl/util>

#include <sstream>
#include <string>
#include <vector>

TEST(ShaderCache, TransformFeedbackEntriesRoundTripSeparatelyFromGraphics)
{
    Babylon::Plugins::ShaderCache::Enable();
    Babylon::Plugins::ShaderCache::Clear();
    const auto disableCache = gsl::finally([] {
        Babylon::Plugins::ShaderCache::Clear();
        Babylon::Plugins::ShaderCache::Disable();
    });

    const std::string vertexSource = "void main() { gl_Position = vec4(0.0); }";
    const std::string fragmentSource = "void main() { gl_FragColor = vec4(1.0); }";
    const std::string feedbackSource = "#version 300 es\nout vec4 outPosition;\nvoid main() { outPosition = vec4(1.0); }\n";
    const std::vector<std::string> varyings{"outPosition"};

    Babylon::Graphics::BgfxShaderInfo graphics{};
    graphics.VertexBytes = {1, 2, 3};
    graphics.FragmentBytes = {4, 5};
    graphics.VertexAttributeLocations = {{"position", 0}};
    graphics.UniformStages = {{"diffuseSampler", 2}};

    Babylon::Graphics::BgfxShaderInfo compute{};
    compute.ComputeBytes = {9, 8, 7, 6};
    compute.UniformStages = {{"depthInput", 5}};
    compute.UniformNames = {{"_12_textureSize", "textureSize"}};

    Babylon::Plugins::ShaderCache::AddShader(vertexSource, fragmentSource, graphics);
    Babylon::Plugins::ShaderCache::AddTransformFeedbackShader(feedbackSource, varyings, compute);

    // Transform feedback and graphics entries use separate key spaces, and varyings are part of the key.
    EXPECT_EQ(Babylon::Plugins::ShaderCache::GetTransformFeedbackShader(vertexSource, varyings), nullptr);
    EXPECT_EQ(Babylon::Plugins::ShaderCache::GetTransformFeedbackShader(feedbackSource, {"other"}), nullptr);
    EXPECT_EQ(Babylon::Plugins::ShaderCache::GetShader(feedbackSource, ""), nullptr);

    std::stringstream stream{std::ios::in | std::ios::out | std::ios::binary};
    ASSERT_EQ(Babylon::Plugins::ShaderCache::Save(stream), 2u);
    Babylon::Plugins::ShaderCache::Clear();
    EXPECT_EQ(Babylon::Plugins::ShaderCache::GetTransformFeedbackShader(feedbackSource, varyings), nullptr);
    ASSERT_EQ(Babylon::Plugins::ShaderCache::Load(stream), 2u);

    std::string crlfFeedbackSource;
    for (const char ch : feedbackSource)
    {
        if (ch == '\n')
        {
            crlfFeedbackSource.push_back('\r');
        }
        crlfFeedbackSource.push_back(ch);
    }
    const auto loadedCompute = Babylon::Plugins::ShaderCache::GetTransformFeedbackShader(crlfFeedbackSource, varyings);
    ASSERT_NE(loadedCompute, nullptr);
    EXPECT_EQ(loadedCompute->ComputeBytes, compute.ComputeBytes);
    EXPECT_TRUE(loadedCompute->VertexBytes.empty());
    EXPECT_TRUE(loadedCompute->FragmentBytes.empty());
    EXPECT_EQ(loadedCompute->UniformStages, compute.UniformStages);
    EXPECT_EQ(loadedCompute->UniformNames, compute.UniformNames);

    const auto loadedGraphics = Babylon::Plugins::ShaderCache::GetShader(vertexSource, fragmentSource);
    ASSERT_NE(loadedGraphics, nullptr);
    EXPECT_EQ(loadedGraphics->VertexBytes, graphics.VertexBytes);
    EXPECT_EQ(loadedGraphics->FragmentBytes, graphics.FragmentBytes);
    EXPECT_TRUE(loadedGraphics->ComputeBytes.empty());
    EXPECT_EQ(loadedGraphics->VertexAttributeLocations, graphics.VertexAttributeLocations);
    EXPECT_EQ(loadedGraphics->UniformStages, graphics.UniformStages);

    std::stringstream resaved{std::ios::in | std::ios::out | std::ios::binary};
    EXPECT_EQ(Babylon::Plugins::ShaderCache::Save(resaved), 2u);
    EXPECT_EQ(resaved.str(), stream.str());
}