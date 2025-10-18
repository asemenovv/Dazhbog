#include "Interval.h"

Interval::Interval(const float min, const float max): m_Min(min), m_Max(max) {
}

float Interval::Size() const {
    return m_Max - m_Min;
}

bool Interval::Contains(const float x) const {
    return m_Min <= x && x <= m_Max;
}

bool Interval::Surrounds(float x) const {
    return m_Min < x && x < m_Max;
}

const Interval Interval::empty    = Interval(+std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
const Interval Interval::universe = Interval(-std::numeric_limits<float>::infinity(), +std::numeric_limits<float>::infinity());
