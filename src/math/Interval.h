#pragma once
#include <limits>


class Interval {
public:
    explicit Interval(float min = +std::numeric_limits<float>::infinity(),
                      float max = -std::numeric_limits<float>::infinity());

    Interval(const Interval &) = delete;

    Interval(Interval &&) = delete;

    float Size() const;

    bool Contains(float x) const;

    bool Surrounds(float x) const;

    static const Interval empty, universe;

private:
    float m_Min;
    float m_Max;
};
