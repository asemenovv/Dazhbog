#include "ImagePostProcessors.h"

#include <png.h>
#include <cstdio>

void Image::WritePng(const std::string& fileName) const
{
    FILE* fp = fopen(fileName.c_str(), "wb");
    if (!fp) { /* handle error */ }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) { /* handle error */ }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) { /* handle error */ }

    if (setjmp(png_jmpbuf(png_ptr))) { /* handle error */ }

    png_init_io(png_ptr, fp);

    png_set_IHDR(png_ptr, info_ptr, Width, Height, 8, PNG_COLOR_TYPE_RGBA,
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);

    uint32_t* pixels = new uint32_t[Width * Height];
    ToRGBA8(pixels);
    for (int y = 0; y < Height; ++y) {
        const auto row = reinterpret_cast<unsigned char*>(&pixels[y * Width * 4]);
        png_write_row(png_ptr, row);
    }

    png_write_end(png_ptr, nullptr);
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);
}

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

BloomProcessor::BloomProcessor(const float threshold, const int levels, const int radius, const float sigma, const float intensity)
    : m_Threshold(threshold), m_Levels(levels), m_Radius(radius), m_Sigma(sigma), m_Intensity(intensity) {
}

void BloomProcessor::ProcessImage(Image &input, Image &output) {
    output.Resize(input.Width, input.Height);

    Image bright {};
    bright.Resize(input.Width, input.Height);
    brightPass(input, bright);

    std::vector<Image> pyramid;
    pyramid.reserve(std::max(m_Levels, 1));
    pyramid.push_back(bright);

    for (int i = 1; i < m_Levels; ++i) {
        Image current {};
        current.Resize(input.Width, input.Height);
        downsample2x(pyramid.back(), current);
        pyramid.push_back(current);
    }

    for (auto& level : pyramid)
    {
        gaussianBlurSeparable(level);
    }

    for (int i = m_Levels - 1; i > 0; --i)
    {
        upsampleAdd(pyramid[i - 1], pyramid[i], 1.0f);
    }

    for (uint32_t y = 0; y < output.Height; y++)
    {
        for (uint32_t x = 0; x < output.Width; x++)
        {
            output.AddColor(x, y, m_Intensity * pyramid[0].GetPixel(x, y));
        }
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
    const uint32_t W = std::max(1, input.Width / 2);
    const uint32_t H = std::max(1, input.Height / 2);
    for (int y=0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            const int x0 = std::min(input.Width - 1, 2 * x);
            const int y0 = std::min(input.Height - 1, 2 * y);
            const int x1 = std::min(input.Width - 1, 2 * x + 1);
            const int y1 = std::min(input.Height - 1, 2 * y + 1);
            const auto outColor = 0.25f * (input.GetPixel(x0, y0) + input.GetPixel(x1, y0) + input.GetPixel(x0, y1) + input.GetPixel(x1, y1));
            output.SetPixel(x, y, outColor);
        }
    }
}

std::vector<float> BloomProcessor::gaussianKernel1D() const
{
    std::vector<float> k(2 * m_Radius + 1);
    float sum = 0.0f;
    for (int i = -m_Radius; i <= m_Radius; ++i) {
        float v = std::exp(-0.5f * (static_cast<float>(i) * static_cast<float>(i))/(m_Sigma * m_Sigma));
        k[i + m_Radius] = v;
        sum += v;
    }
    for (float& v : k) v /= sum;
    return k;
}

void BloomProcessor::gaussianBlurSeparable(Image& input)
{
    if (m_Radius <= 0) return;
    auto k = gaussianKernel1D();
    Image temp {};
    temp.Resize(input.Width, input.Height);

    for (int y = 0; y < input.Height; ++y)
    {
        for (int x = 0; x < input.Width; ++x)
        {
            glm::vec3 acc(0.0f);
            for (int i = -m_Radius; i <= m_Radius; ++i)
            {
                acc += k[i + m_Radius] * input.GetPixel(std::clamp(x + i, 0, static_cast<int>(input.Width) - 1),y);
            }
            temp.SetPixel(x, y, {acc, 1.0f});
        }
    }

    for (int y = 0; y < input.Height; ++y)
    {
        for (int x=0; x < input.Width; ++x) {
            glm::vec3 acc(0.0f);
            for (int i = -m_Radius; i <= m_Radius; ++i)
            {
                acc += k[i + m_Radius] * temp.GetPixel(x,std::clamp(y + i, 0, static_cast<int>(input.Height) - 1));
            }
            input.SetPixel(x, y, {acc, 1.0f});
        }
    }
}

void BloomProcessor::upsampleAdd(Image& big, const Image& small, float gain)
{
    for (int y=0; y < big.Height; ++y) {
        float v = (small.Height>1)
            ? ((static_cast<float>(y) + 0.5f) * static_cast<float>(small.Height) / static_cast<float>(big.Height) - 0.5f) : 0.0f;
        int y0 = std::clamp(static_cast<int>(std::floor(v)), 0, static_cast<int>(small.Height) - 1);
        int y1 = std::min(y0 + 1, static_cast<int>(small.Height) - 1);
        float fy = v - static_cast<float>(y0);

        for (int x=0; x<big.Width; ++x) {
            float u = (small.Width > 1)
                ? ( (static_cast<float>(x) + 0.5f) * static_cast<float>(small.Width) / static_cast<float>(big.Width) - 0.5f ) : 0.0f;
            int x0 = std::clamp(static_cast<int>(std::floor(u)), 0, static_cast<int>(small.Width)-1);
            int x1 = std::min(x0 + 1, static_cast<int>(small.Width) - 1);
            float fx = u - static_cast<float>(x0);

            glm::vec3 c00 = xyz(small.GetPixel(x0, y0));
            glm::vec3 c10 = xyz(small.GetPixel(x1, y0));
            glm::vec3 c01 = xyz(small.GetPixel(x0, y1));
            glm::vec3 c11 = xyz(small.GetPixel(x1, y1));

            glm::vec3 c0 = glm::mix(c00, c10, fx);
            glm::vec3 c1 = glm::mix(c01, c11, fx);
            glm::vec3 c  = glm::mix(c0, c1, fy);

            big.AddColor(x, y, {gain * c, 0.0f});
        }
    }
}
