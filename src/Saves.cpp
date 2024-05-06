#include "Saves.hpp"

#include <fstream>
#include <filesystem>

extern MandelbrotsetConfiguration mandelbrotsetConfiguration;
extern std::filesystem::path savePath;

/*  Configuration file specifications:
    Filename has to end in ".mtc" which stands for Mandelbrotset Thread Configuration.
    The first 34 bytes are always inside the configuration file.
    Header layout:
        byte[0, 1]                           = 'T', 'C'     (char, char)
        byte[ 2,  3,  4,  5,  6,  7 , 8 , 9] = imageWidth   (uint64_t)
        byte[10, 11, 12, 13, 14, 15, 16, 17] = imageHeight  (uint64_t)
        byte[18, 19, 20, 21, 22, 23, 24, 25] = threadWidth  (uint64_t)
        byte[26, 27, 28, 29, 30, 31, 32, 33] = threadHeight (uint64_t)

    TODO: thread completion data standard
*/

// Returns whether or not a valid configuration file exists within savePath/save.mtc.
bool detectConfiguration() {
    std::filesystem::path configurationFilepath = savePath.append("save.mtc");
    if (!std::filesystem::exists(configurationFilepath)) return false;
    std::ifstream configurationFile(configurationFilepath, std::ios::binary);
    if (!configurationFile) return false; // TODO: Add error to logs
    const char headerSize = 2 + sizeof(MandelbrotsetConfiguration);
    char header[headerSize + 1];  // Extra character for null character.
    if (configurationFile.read(header, 2).gcount() != 2) return false;
    if (header != "TC") return false;
    if (configurationFile.read(header + 2, headerSize - 2).gcount() != headerSize - 2) return false;

    configurationFile.close();
    return true;
    // TODO: Validate image thread data
}

// Loads the current state of the computation from the save file directly into mandelbrotsetConfiguration
// There is no checking whether the variables are clean or not, just direct memory access (DANGEROUS)
// This is not portable because save files from different systems might have different endianness.
void loadConfiguration() {
    std::ifstream configurationFile(savePath.append("save.mtc"), std::ios::binary);
    configurationFile.read(nullptr, 2);  // Discard TC header
    configurationFile.read((char*)&mandelbrotsetConfiguration, sizeof(mandelbrotsetConfiguration));
    configurationFile.close();

    // TODO: Read image thread data
}

// Saves the current state of computation, i.e. mandelbrotsetConfiguration directly into the file.
// This is not portable because save files from different systems might have different endianness.
void saveConfiguration() {
    std::ofstream configurationFile(savePath.append("save.mtc"), std::ios::binary);
    configurationFile.write("TC", 2);
    configurationFile.write((char*)&mandelbrotsetConfiguration, sizeof(mandelbrotsetConfiguration));
    configurationFile.close();

    // TODO: Save image thread data
}
