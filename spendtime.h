#pragma once


#include <chrono>

#include "log.h"

class SpendTime
{
public:
    SpendTime()
    {
        m_start = std::chrono::high_resolution_clock::now();
    }

    ~SpendTime()
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::duration<double, std::chrono::minutes::period>>(currentTime - m_start);

        BOOST_LOG(queryLog::get()) << "durations: took " << elapsed.count() << " mins\n";
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
};