Да — это «пересвет» из-за того, что ты выводишь **линейную HDR-яркость** напрямую в 8-битный экран без тонмаппинга/гаммы.  
Эмиссивный материал даёт значения > 1, всё клипается в белый.  
Решение в 3 шага:

---

## 1️⃣ Копи HDR в float, мэпь только перед выводом
Храни аккумулированный цвет пикселя в `glm::vec3` (float), дели на число сэмплов, **не клампи** в процессе трассировки.  
Мэппинг делай в конце кадра/сэмпла.

---

## 2️⃣ Exposure + Tone Mapping + Gamma
Добавь экспозицию, тонмапер (лучше **ACES**), затем гамма 2.2 (или sRGB).

```cpp
// экспозиция в EV: +1 = ×2, −1 = ×0.5
inline glm::vec3 ApplyExposure(const glm::vec3& c, float exposureEV) {
    float k = std::exp2(exposureEV);
    return c * k;
}

// Narkowicz ACES (быстрый и приятный)
inline glm::vec3 TonemapACES(const glm::vec3& x) {
    const glm::vec3 a(2.51f), b(0.03f), c(2.43f), d(0.59f), e(0.14f);
    return glm::clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0f, 1.0f);
}

inline glm::vec3 LinearToSRGB(const glm::vec3& c) {
    // аппрокс. гамма 2.2 (можно заменить точной sRGB кривой)
    return glm::pow(glm::clamp(c, 0.0f, 1.0f), glm::vec3(1.0f / 2.2f));
}
```

В конце рендеринга (например, в `perPixel`):

```cpp
glm::vec3 hdr = rayColor;             // линейный HDR (не клампить!)
hdr = ApplyExposure(hdr, exposureEV); // например, −1 … +2 EV
glm::vec3 ldr = TonemapACES(hdr);     // 0 … 1
glm::vec3 srgb = LinearToSRGB(ldr);
return { srgb, 1.0f };
```

---

## 3️⃣ Эмиссия должна быть в линейном цвете
Твои значения `(1.0, 0.706, 0.422)` — это, скорее всего, **sRGB**.  
Для физически корректной энергии их надо перевести в линейное пространство:

```cpp
inline float SRGBToLinear(float u) {
    return (u <= 0.04045f) ? (u / 12.92f)
                           : std::pow((u + 0.055f) / 1.055f, 2.4f);
}
inline glm::vec3 SRGBToLinear(const glm::vec3& c) {
    return { SRGBToLinear(c.r), SRGBToLinear(c.g), SRGBToLinear(c.b) };
}

// пример использования:
glm::vec3 warm_srgb   = { 1.0f, 0.706f, 0.422f };
glm::vec3 warm_linear = SRGBToLinear(warm_srgb);
const auto lightMat = m_Scene->Add(
    new DiffuseLightMaterial(warm_linear, 3.0f) // 3.0 = интенсивность
);
```

> Если оставить sRGB как линейный, ты «перенадуваешь» энергию — пересвет усиливается.

---

## 💡 Дополнительно

- **Подбор экспозиции** — начни с `exposureEV = −1.0 … +1.0`.  
- **Fireflies** — добавь мягкий clamp (яркость ≤ 3–5) и/или next event estimation + MIS.  
- **Интенсивность** — думай об эмиссии как о *radiance per area*: чем меньше площадь — тем выше яркость.  
- **Клампить цвета** только после тонмапа.

---

С этими правками свет лампы перестанет быть белым пятном:  
в хайлайтах сохранится тёплый оттенок, а общая сцена будет управляться экспозицией.
