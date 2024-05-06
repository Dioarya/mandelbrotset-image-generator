#include "Mandelbrotset.hpp"
#include "Saves.hpp"
#include <iostream>

MandelbrotsetConfiguration mandelbrotsetConfiguration = {
    1920ULL,
    1080ULL,

    1ULL,
    1ULL,

    -20.0L / 9.0L,
    20.0L / 9.0L,
    1.25L,
    -1.25L,

    1000,
    1 << 8,
    1 / 100000000000000.0L,
    200
};

std::filesystem::path savePath = "mandelbrotset/";

int main() {
    std::cout << "startReal: " << mandelbrotsetConfiguration.startReal << std::endl;
    std::cout << "endReal: " << mandelbrotsetConfiguration.endReal << std::endl;
    std::cout << "startImag: " << mandelbrotsetConfiguration.startImag << std::endl;
    std::cout << "endImag: " << mandelbrotsetConfiguration.endImag << std::endl;

    Sample samples[8];
    computeIterationsVector(1920 / 2, 1080 / 2, samples);
    for (const Sample& sample : samples) {
        std::cout << sample.cReal << " " << sample.cImag << " " << sample.iterations << " " << sample.finalMagnitude2 << std::endl;
    }
}