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
            const glm::vec4 color = input.GetPixel(x, y);
            output.SetPixel(x, y, glm::vec4(
                linearToGamma(color.r),
                linearToGamma(color.g),
                linearToGamma(color.b),
                1.0f)
            );
        }
    }
}

double GammaCorrectionProcessor::linearToGamma(const double linearComponent) {
    if (linearComponent > 0)
        return std::sqrt(linearComponent);
    return 0;
}
