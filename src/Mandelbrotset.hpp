#include <cstdint>

// Represents the final data after the mandelbrot iteration at a single point.
struct Sample {
    double cReal;
    double cImag;
    int iterations = 0;
    double finalMagnitude2 = 0;
};

// Takes in x and y as image pixels and outputs them in samples output array
void computeIterationsVector(uint64_t x, uint64_t y, Sample outSamples[8]) noexcept;