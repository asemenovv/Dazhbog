//
// Created by Alex Semenov on 10/20/25.
//

#ifndef DAZHBOG_TIMER_H
#define DAZHBOG_TIMER_H
#include <chrono>

namespace Utils
{
    class Timer
    {
    public:
        Timer() = default;

        void Start();

        uint64_t StopAndGetTime();
    private:
        bool m_IsRunning = false;
        uint64_t m_ElapsedTimeMs = 0;
        std::chrono::steady_clock::time_point m_StartTime;
    };
}


#endif //DAZHBOG_TIMER_H