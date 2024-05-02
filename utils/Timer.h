#pragma once

#include <iostream>
#include <chrono>

struct Timer
{
    std::chrono::high_resolution_clock::time_point start, end;
    std::chrono::duration<double> elapsed;
    std::string_view name;

    Timer()
    : name("Timer")
    {
        start = std::chrono::high_resolution_clock::now();
    }

    Timer(std::string_view timerName)
    : name(timerName)
    {
        start = std::chrono::high_resolution_clock::now();
    }

    ~Timer()
    {
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;

        double ms = elapsed.count() * 1000;

        std::cout << name << " took: " << ms << "ms" << std::endl;
    }
};