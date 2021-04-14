#pragma once

//From @mcleary https://gist.github.com/mcleary/b0bf4fa88830ff7c882d

#include <chrono>

class Timer
{
public:
    void Start();
    void Stop();

    double ElapsedMilliseconds() const;
    double ElapsedSeconds() const;

private:
    std::chrono::time_point<std::chrono::system_clock> m_StartTime;
    std::chrono::time_point<std::chrono::system_clock> m_EndTime;
    bool                                               m_bRunning = false;
};
