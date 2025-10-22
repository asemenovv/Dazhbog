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

void GammaCorrectionProcessor::ProcessImage(Image &input, Image &output) {
    output.Resize(input.Width, input.Height);
    for (uint32_t y = 0; y < input.Height; y++) {
        for (uint32_t x = 0; x < input.Width; x++) {
            const glm::vec color = input.GetPixel(x, y);
            const auto result = glm::vec4(
                glm::pow(
                    glm::clamp(glm::vec3(color), 0.0f, 1.0f),
                    glm::vec3(1.0f / 2.2f)),
                    color.a);
            output.SetPixel(x, y, result);
        }
    }
}

HDRProcessor::HDRProcessor(float exposure): m_Exposure(exposure) {
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
