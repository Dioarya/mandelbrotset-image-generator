#ifndef MANDELBROTSET_HPP_INCLUDED
#define MANDELBROTSET_HPP_INCLUDED
#include <cstdint>

// Minimum amount of information to uniquely represent a pixel
struct Sample {
    double cReal;
    double cImag;
    int64_t iterations;
    double finalMagnitude2;

    Sample() : cReal(0), cImag(0), iterations(0), finalMagnitude2(0) {}
};

// Takes in x and y as image pixels and outputs them in samples output array
void computeIterationsVector(uint64_t x, uint64_t y, Sample outSamples[8]) noexcept;

#endif  // MANDELBROTSET_HPP_INCLUDED