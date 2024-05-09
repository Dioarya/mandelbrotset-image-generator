#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "Mandelbrotset.hpp"
#include "Saves.hpp"

MandelbrotsetConfiguration mConfig = {
    .startReal = -20.0L / 9.0L,
    .endReal = 20.0L / 9.0L,
    .startImag = 1.25L,
    .endImag = -1.25L,

    .maxIterations = 1000ULL,
    .bailoutRadius = 1 << 8,
    .periodicityPrecision2 = 1E-14L,
    .periodicitySavePeriod = 200
};
TileConfiguration tConfig = {
    .imageWidth = 1920ULL,
    .imageHeight = 1080ULL,

    .tileGridWidth = 64ULL,
    .tileGridHeight = 64ULL,

    .threadGridWidth = 128ULL,
    .threadGridHeight = 72ULL
};
ProgressConfiguration pConfig = {
    .currentTile = 0ULL,
    .tileCount = tConfig.tileGridWidth * tConfig.tileGridHeight,
    .threadCount = tConfig.threadGridWidth * tConfig.threadGridHeight,
    .tileCompletion = (new unsigned char[(pConfig.tileCount + 7) / 8]{0}),
    .threadCompletion = (new unsigned char[(pConfig.threadCount + 7) / 8]{0})
};

std::filesystem::path savePath = "mandelbrotset/";

int main() {
    std::cout << "startReal: " << mConfig.startReal << std::endl;
    std::cout << "endReal: " << mConfig.endReal << std::endl;
    std::cout << "startImag: " << mConfig.startImag << std::endl;
    std::cout << "endImag: " << mConfig.endImag << std::endl;

    Sample samples[8];
    computeIterationsVector(1920 / 2, 1080 / 2, samples);
    for (const Sample& sample : samples) {
        std::cout << sample.cReal << " " << sample.cImag << " " << sample.iterations << " " << sample.finalMagnitude2 << std::endl;
    }
}