# Реализация эффекта Bloom (свечения) на CPU без шейдеров

Эффект **bloom / glow** создаёт ощущение свечения от ярких объектов.  
Ниже — полностью рабочий вариант на C++/GLM, подходящий для CPU path tracer'а.

---

## 💡 Принцип работы

1. **Bright pass** — выделяем яркие пиксели (порог `threshold` в HDR).
2. **Downsample chain** — уменьшаем изображение (в 2, 4, 8 раз).
3. **Separable Gaussian blur** — размываем каждый уровень.
4. **Upsample + Add** — возвращаем уровни обратно и суммируем.
5. **Additive combine** — добавляем bloom к HDR буферу.
6. **Тонмап + гамма** — применяем после bloom.

---

## 🧩 Код реализации (C++ / GLM)

```cpp
// BloomCPU.h
#pragma once
#include <vector>
#include <algorithm>
#include <cmath>
#include <glm/glm.hpp>

struct ImageF {
    int w=0, h=0;
    std::vector<glm::vec3> px; // size = w*h
    ImageF() = default;
    ImageF(int W, int H) : w(W), h(H), px(W*H, glm::vec3(0)) {}
    glm::vec3& at(int x, int y) { return px[x + y*w]; }
    const glm::vec3& at(int x, int y) const { return px[x + y*w]; }
};

inline float Luminance(const glm::vec3& c) {
    return 0.2126f*c.r + 0.7152f*c.g + 0.0722f*c.b;
}

// Bright pass
inline ImageF BrightPass(const ImageF& src, float threshold) {
    ImageF out(src.w, src.h);
    for (int y=0; y<src.h; ++y)
        for (int x=0; x<src.w; ++x) {
            glm::vec3 c = src.at(x,y);
            out.at(x,y) = (Luminance(c) > threshold) ? c : glm::vec3(0.0f);
        }
    return out;
}

// Downsample 2x
inline ImageF Downsample2x(const ImageF& src) {
    int W = std::max(1, src.w/2);
    int H = std::max(1, src.h/2);
    ImageF out(W,H);
    for (int y=0; y<H; ++y)
        for (int x=0; x<W; ++x) {
            int x0 = std::min(src.w-1, 2*x);
            int y0 = std::min(src.h-1, 2*y);
            int x1 = std::min(src.w-1, 2*x+1);
            int y1 = std::min(src.h-1, 2*y+1);
            out.at(x,y) = 0.25f * (src.at(x0,y0) + src.at(x1,y0) + src.at(x0,y1) + src.at(x1,y1));
        }
    return out;
}

// Upsample + add
inline void UpsampleAdd(ImageF& big, const ImageF& small, float gain=1.0f) {
    for (int y=0; y<big.h; ++y) {
        float v = (small.h>1) ? ( (y + 0.5f) * small.h / big.h - 0.5f ) : 0.0f;
        int y0 = std::clamp((int)std::floor(v), 0, small.h-1);
        int y1 = std::min(y0+1, small.h-1);
        float fy = v - y0;

        for (int x=0; x<big.w; ++x) {
            float u = (small.w>1) ? ( (x + 0.5f) * small.w / big.w - 0.5f ) : 0.0f;
            int x0 = std::clamp((int)std::floor(u), 0, small.w-1);
            int x1 = std::min(x0+1, small.w-1);
            float fx = u - x0;

            glm::vec3 c00 = small.at(x0,y0);
            glm::vec3 c10 = small.at(x1,y0);
            glm::vec3 c01 = small.at(x0,y1);
            glm::vec3 c11 = small.at(x1,y1);

            glm::vec3 c0 = glm::mix(c00, c10, fx);
            glm::vec3 c1 = glm::mix(c01, c11, fx);
            glm::vec3 c  = glm::mix(c0, c1, fy);

            big.at(x,y) += gain * c;
        }
    }
}

// Gaussian kernel
inline std::vector<float> GaussianKernel1D(int radius, float sigma) {
    std::vector<float> k(2*radius+1);
    float sum=0.0f;
    for (int i=-radius; i<=radius; ++i) {
        float v = std::exp(-0.5f * (i*i)/(sigma*sigma));
        k[i+radius] = v;
        sum += v;
    }
    for (float& v : k) v /= sum;
    return k;
}

// Separable blur
inline void GaussianBlurSeparable(ImageF& img, int radius, float sigma) {
    if (radius <= 0) return;
    auto k = GaussianKernel1D(radius, sigma);
    ImageF temp(img.w, img.h);

    // горизонталь
    for (int y=0; y<img.h; ++y)
        for (int x=0; x<img.w; ++x) {
            glm::vec3 acc(0.0f);
            for (int i=-radius; i<=radius; ++i)
                acc += k[i+radius] * img.at(std::clamp(x+i,0,img.w-1),y);
            temp.at(x,y) = acc;
        }

    // вертикаль
    for (int y=0; y<img.h; ++y)
        for (int x=0; x<img.w; ++x) {
            glm::vec3 acc(0.0f);
            for (int i=-radius; i<=radius; ++i)
                acc += k[i+radius] * temp.at(x,std::clamp(y+i,0,img.h-1));
            img.at(x,y) = acc;
        }
}

// Основной пайплайн Bloom
struct BloomSettings {
    float threshold   = 1.0f;
    float intensity   = 0.6f;
    int   levels      = 4;
    int   blurRadius  = 6;
    float blurSigma   = 4.0f;
};

inline void ApplyBloom(ImageF& hdrColor, const BloomSettings& s) {
    ImageF bright = BrightPass(hdrColor, s.threshold);
    std::vector<ImageF> pyr;
    pyr.reserve(std::max(s.levels,1));
    pyr.push_back(std::move(bright));
    for (int i=1; i<s.levels; ++i)
        pyr.push_back(Downsample2x(pyr.back()));

    for (auto& level : pyr)
        GaussianBlurSeparable(level, s.blurRadius, s.blurSigma);

    for (int i=s.levels-1; i>0; --i)
        UpsampleAdd(pyr[i-1], pyr[i], 1.0f);

    for (int i=0; i<hdrColor.w*hdrColor.h; ++i)
        hdrColor.px[i] += s.intensity * pyr[0].px[i];
}
```

---

## 📋 Использование

```cpp
BloomSettings bloom;
bloom.threshold = 1.2f;
bloom.intensity = 0.6f;
bloom.levels    = 4;
bloom.blurRadius= 6;
bloom.blurSigma = 4.0f;

ApplyBloom(hdr, bloom);
```

---

## ⚙️ Рекомендованные параметры

| Параметр | Значение |
|-----------|-----------|
| `threshold` | 1.0–1.5 |
| `intensity` | 0.4–0.8 |
| `levels` | 3–5 |
| `blurRadius` | 4–8 |
| `blurSigma` | 3–5 |

---

## 🚀 Оптимизации

- Разделить блюр по потокам (`std::thread`, `TBB`, `QtConcurrent`).
- Хранить pyramid в `RGBA16F` при переходе на GPU.
- Можно заменить Gaussian на Kawase blur (быстрее и мягче).

---

Эта реализация работает целиком на CPU и даёт мягкое, реалистичное свечение даже без шейдеров.
