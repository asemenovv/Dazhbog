#include "ImagePostProcessors.h"

AverageFramesProcessor::AverageFramesProcessor(const uint32_t frameIndex) : m_FrameIndex(frameIndex) {
}

void AverageFramesProcessor::ProcessImage(Image &input, Image &output) {
    output.Resize(input.Width, input.Height);
    for (uint32_t y = 0; y < input.Height; y++) {
        for (uint32_t x = 0; x < input.Width; x++) {
            glm::vec4 accumulatedColor = input.GetPixel(x, y);
            accumulatedColor /= static_cast<float>(m_FrameIndex);
            output.SetPixel(x, y, accumulatedColor);
        }
    }
}

GammaCorrectionProcessor::GammaCorrectionProcessor(const float gamma): m_Gamma(gamma) {
}

void GammaCorrectionProcessor::ProcessImage(Image &input, Image &output) {
    output.Resize(input.Width, input.Height);
    for (uint32_t y = 0; y < input.Height; y++) {
        for (uint32_t x = 0; x < input.Width; x++) {
            const glm::vec color = input.GetPixel(x, y);
            const auto result = glm::vec4(
                glm::pow(
                    glm::clamp(glm::vec3(color), 0.0f, 1.0f),
                    glm::vec3(1.0f / m_Gamma)),
                    color.a);
            output.SetPixel(x, y, result);
        }
    }
}

HDRProcessor::HDRProcessor(const float exposure): m_Exposure(exposure) {
}

void HDRProcessor::ProcessImage(Image &input, Image &output) {
    output.Resize(input.Width, input.Height);
    const float k = std::exp2(m_Exposure);
    for (uint32_t y = 0; y < input.Height; y++) {
        for (uint32_t x = 0; x < input.Width; x++) {
            glm::vec4 color = input.GetPixel(x, y);
            output.SetPixel(x, y, k * color);
        }
    }
}

void TonemapACESProcessor::ProcessImage(Image &input, Image &output) {
    output.Resize(input.Width, input.Height);
    constexpr glm::vec3 a(2.51f), b(0.03f), c(2.43f), d(0.59f), e(0.14f);
    for (uint32_t y = 0; y < input.Height; y++) {
        for (uint32_t x = 0; x < input.Width; x++) {
            auto color = glm::vec3(input.GetPixel(x, y));
            glm::vec3 result = glm::clamp((color * (a * color + b)) / (color * (c * color + d) + e), 0.0f, 1.0f);
            output.SetPixel(x, y, glm::vec4(result, input.GetPixel(x, y).a));
        }
    }
}

BloomProcessor::BloomProcessor(const float threshold, const uint32_t levels): m_Threshold(threshold), m_Levels(levels) {
}

void BloomProcessor::ProcessImage(Image &input, Image &output) {
    output.Resize(input.Width, input.Height);

    Image bright {};
    bright.Resize(input.Width, input.Height);
    brightPass(input, bright);

    std::vector<Image> pyramid;
    pyramid.reserve(std::max(m_Levels, 1u));
    pyramid.push_back(std::move(bright));

    for (int i = 1; i < m_Levels; ++i) {
        Image current {};
        current.Resize(input.Width, input.Height);
        downsample2x(pyramid.back(), current);
        pyramid.push_back(current);
    }
}

float BloomProcessor::luminance(const glm::vec3 &color) {
    return 0.2126f * color.r + 0.7152f * color.g + 0.0722f * color.b;
}

void BloomProcessor::brightPass(const Image &input, const Image &output) {
    for (uint32_t y = 0; y < input.Height; y++) {
        for (uint32_t x = 0; x < input.Width; x++) {
            glm::vec4 color = input.GetPixel(x, y);
            const glm::vec4 outColor = luminance(glm::xyz(color)) > m_Threshold
                ? color : glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            output.SetPixel(x, y, outColor);
        }
    }
}

void BloomProcessor::downsample2x(const Image &input, const Image &output) {
    const uint32_t W = std::max(1u, input.Width / 2);
    const uint32_t H = std::max(1u, input.Height / 2);
    for (int y=0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            const int x0 = std::min(input.Width - 1, 2u * x);
            const int y0 = std::min(input.Height - 1, 2u * y);
            const int x1 = std::min(input.Width - 1, 2u * x + 1u);
            const int y1 = std::min(input.Height - 1, 2u * y + 1u);
            const auto outColor = 0.25f * (input.GetPixel(x0, y0) + input.GetPixel(x1, y0) + input.GetPixel(x0, y1) + input.GetPixel(x1, y1));
            output.SetPixel(x, y, outColor);
        }
    }
}
