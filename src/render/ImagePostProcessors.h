#pragma once

#include <glm/glm.hpp>

struct Image {
    uint32_t Width = 0, Height = 0;
    glm::vec4* Data;

    void Resize(const uint32_t width, const uint32_t height) {
        if (width != Width || height != Height) {
            delete[] Data;
            Data = new glm::vec4[width * height];
        }
        Width = width;
        Height = height;
    }

    glm::vec4 GetPixel(const uint32_t x, const uint32_t y) const {
        return Data[y * Width + x];
    }

    void SetPixel(const uint32_t x, const uint32_t y, const glm::vec4 color) const {
        Data[y * Width + x] = color;
    }

    void AddColor(const uint32_t x, const uint32_t y, const glm::vec4& color) const {
        Data[x + y * Width] += color;
    }

    void ZeroAll() const {
        memset(Data, 0, Width * Height * sizeof(glm::vec4));
    }

    void ToRGBA8(uint32_t* output) const {
        for (uint32_t y = 0; y < Height; y++) {
            for (uint32_t x = 0; x < Width; x++) {
                output[y * Width + x] = Vec4ToRGBA8(glm::clamp(GetPixel(x, y), 0.0f, 1.0f));
            }
        }
    }

    static uint32_t Vec4ToRGBA8(const glm::vec4 color) {
        const auto r = static_cast<uint8_t>(color.r * 255.0f);
        const auto g = static_cast<uint8_t>(color.g * 255.0f);
        const auto b = static_cast<uint8_t>(color.b * 255.0f);
        const auto a = static_cast<uint8_t>(color.a * 255.0f);
        return a << 24 | b << 16 | g << 8 | r;
    }
};

class ImagePostProcessor {
public:
    virtual ~ImagePostProcessor() = default;

    virtual void ProcessImage(Image &input, Image &output) = 0;
};

class AverageFramesProcessor final : public ImagePostProcessor {
public:
    explicit AverageFramesProcessor(uint32_t frameIndex);

    void ProcessImage(Image &input, Image &output) override;
private:
    uint32_t m_FrameIndex = 0;
};

class GammaCorrectionProcessor final : public ImagePostProcessor {
public:
    explicit GammaCorrectionProcessor(float gamma = 2.2f);

    void ProcessImage(Image &input, Image &output) override;
private:
    float m_Gamma;
};

class HDRProcessor final : public ImagePostProcessor {
public:
    explicit HDRProcessor(float exposure);

    void ProcessImage(Image &input, Image &output) override;
private:
    float m_Exposure;
};

class TonemapACESProcessor final : public ImagePostProcessor {
public:
    TonemapACESProcessor() = default;

    void ProcessImage(Image &input, Image &output) override;
};

class BloomProcessor final : public ImagePostProcessor {
public:
    explicit BloomProcessor(float threshold, uint32_t levels);

    void ProcessImage(Image &input, Image &output) override;
private:
    float luminance(const glm::vec3& color);

    void brightPass(const Image &input, const Image &output);

    void downsample2x(const Image &input, const Image &output);

    float m_Threshold;
    uint32_t m_Levels;
};
