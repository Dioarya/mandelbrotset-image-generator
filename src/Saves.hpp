#include <cstdint>
#include <filesystem>

// Minimum amount of information to get the same image output.
struct MandelbrotsetConfiguration {
    // Image output configurations
    uint64_t imageWidth;
    uint64_t imageHeight;

    // Thread configurations
    uint64_t threadWidth;
    uint64_t threadHeight;

    // Mandelbrotset configurations.
    double startReal;
    double endReal;
    double startImag;
    double endImag;

    uint64_t maxIterations;
    double bailoutRadius;
    double periodicityPrecision2;
    uint64_t periodicitySavePeriod;
};

// TODO: Add struct for current computation progress (data for tiles, current tile, current tile thread completion... and more probably)

// Returns whether or not a save was detected within savePath
bool detectConfiguration();

// Loads the configuration file within savePath
void loadConfiguration();

// Saves the configuration into a file inside savePath
void saveConfiguration();