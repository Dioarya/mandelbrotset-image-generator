#include "Mandelbrotset.hpp"
#include "Saves.hpp"

#include <omp.h>

#include <cmath>
#include <cstdint>

extern MandelbrotsetConfiguration mConfig;
extern TileConfiguration tConfig;
extern ProgressConfiguration pConfig;

void tileGenerator(uint64_t tileIndex, unsigned char* output) noexcept {
    const uint64_t tileWidth     = tConfig.tileWidth();
    const uint64_t tileHeight    = tConfig.tileHeight();
    const uint64_t threadWidth   = tConfig.threadWidth();
    const uint64_t threadHeight  = tConfig.threadHeight();

    const uint64_t tileX         = tileIndex % tConfig.tileGridWidth;
    const uint64_t tileY         = tileIndex / tConfig.tileGridWidth;
    const uint64_t tileXOffset   = tileWidth * tileX;
    const uint64_t tileYOffset   = tileHeight * tileY;

    omp_set_num_threads(pConfig.threadsUsed);
    #pragma omp parallel for schedule(dynamic, 1)
    for (uint64_t threadIndex = 0; threadIndex < pConfig.threadCount; threadIndex++) {
        const uint64_t threadX       = threadIndex % tConfig.threadGridWidth;
        const uint64_t threadY       = threadIndex / tConfig.threadGridWidth;
        const uint64_t threadXOffset = threadWidth * threadX;
        const uint64_t threadYOffset = threadHeight * threadY;

        Sample samples[8];
        for (uint64_t j = 0; j < threadHeight; j++) {
            const uint64_t y = tileYOffset + threadYOffset + j;
            for (uint64_t i = 0; i < threadWidth; i += 8) {
                const uint64_t x = tileXOffset + threadXOffset + i;

                computeIterationsVector(x, y, samples);

                uint64_t outX = i;
                const uint64_t &outY = j;
                int sampleIndex = 0;
                for (; sampleIndex < 8 && outX < threadWidth; outX++) {
                    const uint64_t pixelIndex = ((threadYOffset + outY) * tileWidth + (threadXOffset + outX));
                    Sample &sample = samples[sampleIndex];

                    char brightness = sample.iterations == mConfig.maxIterations ? 0 : std::min(255LL * sample.iterations / 200LL, 255LL);
                    output[pixelIndex] = brightness;
                    sampleIndex++;
                }
            }
        }
    }
}