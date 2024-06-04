#pragma once
#include <chrono>

class CTimer {
    static CTimer *instance;


    bool isRunning = false;
    bool isReady = true;

    CTimer() {}

    public:
    CTimer(CTimer &other) = delete;
    void operator=(const CTimer &) = delete;
    static CTimer *GetInstance();

    void start();
    std::chrono::milliseconds end();
    void reset();

    private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    std::chrono::milliseconds lastTime;
};
