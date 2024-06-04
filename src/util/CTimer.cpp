#include "CTimer.h"

#include <iostream>
#include <ctime>
#include <SDL2/SDL.h>

/* CTimer is a singleton class*/
CTimer* CTimer::instance = 0;

CTimer *CTimer::GetInstance(){
    if (!instance){
    instance = new CTimer();
    }
    return instance;
}

// Start the timer
void CTimer::start() {
    if (isRunning || !isReady) return;
    // Don't do anything if already running or if reset() hasn't been called yet

    isRunning = true;
    startTime = std::chrono::high_resolution_clock::now();
};

// Stop the timer and record the time elapsed
std::chrono::milliseconds CTimer::end() {
    if (!isRunning)
        return lastTime;
    // Don't do anything if not already running

    isRunning = false;
    isReady = false;

    endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> seconds = endTime - startTime; // Find duration between start and end
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(seconds);  // Cast time elapsed to milliseconds
    lastTime = ms;

    //std::cout << seconds.count() << "s\n";
    //std::cout << ms.count() << "ms\n";
    isReady = true;
    return ms;
};

void CTimer::reset() {
    isRunning = false;
    isReady = true;
}
