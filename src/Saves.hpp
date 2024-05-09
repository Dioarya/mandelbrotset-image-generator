#ifndef SAVES_HPP_INCLUDED
#define SAVES_HPP_INCLUDED
#include <cstdint>
#include <filesystem>

/*
Configuration file specifications:

Mandelbrotset configuration:
    Filename has to end in ".mc" which stands for Mandelbrotset Configuration.
    Header byte layout:
        Magic numbers:
        byte[0, 1]                           = 'M' (0x4d), 'C' (0x43) (char, char)

        Image output cofigurations:
        byte[ 2,  3,  4,  5,  6,  7 , 8 , 9] = imageWidth             (uint64_t)
        byte[10, 11, 12, 13, 14, 15, 16, 17] = imageHeight            (uint64_t)

        Mandelbrotset position configurations:
        byte[18, 19, 20, 21, 22, 23, 24, 25] = startReal              (double)
        byte[26, 27, 28, 29, 30, 31, 32, 33] = endReal                (double)
        byte[34, 35, 36, 37, 38, 39, 40, 41] = startImag              (double)
        byte[42, 43, 44, 45, 46, 47, 48, 49] = endImag                (double)

        Mandelbrotset iterating configurations:
        byte[50, 51, 52, 53, 54, 55, 56, 57] = maxIterations          (uint64_t)
        byte[58, 59, 60, 61, 62, 63, 64, 65] = bailoutRadius          (double)
        byte[66, 67, 68, 69, 70, 71, 72, 73] = periodicityPrecision2  (double)
        byte[74, 75, 76, 77, 78, 79, 80, 81] = periodicitySavePeriod  (uint64_t)

Tile configuration:
    Filename has to end in ".mtc" which stands for Mandelbrotset Tile Configuration.
    Header byte layout:
        Magic numbers:
        byte[0, 1]                           = 'T' (0x54), 'C' (0x43) (char, char)

        Image size configurations:
        byte[ 2,  3,  4,  5,  6,  7,  8,  9] = tileGridWidth          (uint64_t)
        byte[10, 11, 12, 13, 14, 15, 16, 17] = tileGridHeight         (uint64_t)


        Tile grid configurations:
        byte[18, 19, 20, 21, 22, 23, 24, 25] = threadGridWidth        (uint64_t)
        byte[26, 27, 28, 29, 30, 31, 32, 33] = threadGridHeight       (uint64_t)

        Thread configurations:
        byte[34, 35, 36, 37, 38, 39, 40, 41] = startReal              (double)
        byte[42, 43, 44, 45, 46, 47, 48, 49] = endReal                (double)

Progress configuration:
    Filename has to end in ".mpc" which stands for Mandelbrotset Progress Configuration.
    Header byte layout:
        Magic numbers:
        byte[0, 1]                           = 'P' (0x50), 'C' (0x43) (char, char)

        Progress:
        byte[ 2,  3,  4,  5,  6,  7,  8,  9] = currentTile            (uint64_t)

        Array lengths:
        byte[10, 11, 12, 13, 14, 15, 16, 17] = threadCount            (uint64_t)
        byte[18, 19, 20, 21, 22, 23, 24, 25] = threadCount            (uint64_t)

        Arrays:
        byte[?, ..., ?]                      = tileCompletion         (unsigned char[]) Note: size of this array is ⌈tileCount / 8⌉, i.e. (tileCount + 7) / 8
        byte[?, ..., ?]                      = threadCompletion       (unsigned char[]) Note: size of this array is ⌈threadCount / 8⌉, i.e. (threadCount + 7) / 8
*/

enum ConfigurationType {
    Null,
    Mandelbrotset,
    Tile,
    Progress
};

// Minimum amount of information for same mandelbrotset position and quality
struct MandelbrotsetConfiguration {
    // Left-most real in image
    const double startReal;
    // Right-most real in image
    const double endReal;
    // Top-most imaginary in image
    const double startImag;
    // Bottom-most imaginary in image
    const double endImag;

    // Maximum amount of iterations, iteration counts are in the interval [0, maxIterations]
    const uint64_t maxIterations;
    // Terminates further iterations when magnitude of the complex number exceeds this limit
    const double bailoutRadius;
    // Radius of termination-circle for periodicity checks. If the complex number's resulting positions after iterations is within this radius around the last position captured by periodicitySavePeriod, then terminate further iterations
    const double periodicityPrecision2;
    // The period to wait before saving the current complex number's position to compare to with subsequent iterations of the complex number
    const uint64_t periodicitySavePeriod;
};

// Minimum amount of information for the same set of image, tiles and threads
struct TileConfiguration {
    // Width of total image in pixels
    const uint64_t imageWidth;
    // Height of total image in pixels
    const uint64_t imageHeight;

    // Number of tiles horizontally
    const uint64_t tileGridWidth;
    // Number of tiles vertically
    const uint64_t tileGridHeight;

    // Number of threads horizontally per tile
    const uint64_t threadGridWidth;
    // Number of threads horizontally per tile
    const uint64_t threadGridHeight;
};

// Minimum amount of information to continue progressing towards the final image
struct ProgressConfiguration {
    // Current tile index
    uint64_t currentTile;
    // Total number of tiles used
    const uint64_t tileCount;
    // Total number of threads used
    const uint64_t threadCount;
    // Tile completion bool/bit array represented using an unsigned char array. This array has the size ⌈tileCount / 8⌉, i.e. (tileCount + 7) / 8. Each unsigned char represents 8 booleans
    unsigned char *tileCompletion;
    // Thread completion bool/bit array represented using an unsigned char array. This array has the size ⌈threadCount / 8⌉, i.e. (threadCount + 7) / 8. Each unsigned char represents 8 booleans
    unsigned char *threadCompletion;

    ~ProgressConfiguration() {
        delete[] tileCompletion;
        delete[] threadCompletion;
    }
};

// Returns the configuration type stored within filepath's contents.
ConfigurationType getConfigurationType(std::filesystem::path filepath);

// Returns whether or not filepath has the correct data structure for specified configuration type.
bool detectConfiguration(std::filesystem::path filepath, ConfigurationType type);

// Loads filepath directly into memory assuming it is of the specified type.
void loadConfiguration(std::filesystem::path filepath, ConfigurationType type);

void detectLoadConfiguration(std::filesystem::path filepath);

// Saves the configuration to filepath, saving which type is based on the specified type.
void saveConfiguration(std::filesystem::path filepath, ConfigurationType type);

#endif  // SAVES_HPP_INCLUDED