#include "Timer.h"

#include <chrono>

namespace Utils
{
    void Timer::Start()
    {
        m_StartTime = std::chrono::high_resolution_clock::now();
        m_IsRunning = true;
    }

    uint64_t Timer::StopAndGetTime()
    {
        if (m_IsRunning)
        {
            std::chrono::steady_clock::time_point endTime = std::chrono::high_resolution_clock::now();
            const auto elapsedTime = endTime - m_StartTime;
            m_ElapsedTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count();
            m_IsRunning = false;
        }
        return m_ElapsedTimeMs;
    }
}
