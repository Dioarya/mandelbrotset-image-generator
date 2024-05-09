#include "Saves.hpp"

#include <string.h>

#include <filesystem>
#include <fstream>

extern MandelbrotsetConfiguration mConfig;
extern TileConfiguration tConfig;
extern ProgressConfiguration pConfig;
extern std::filesystem::path savePath;

// Returns the type inferred from filepath's first two bytes. Can return Null. (ConfigurationType)
inline ConfigurationType getConfigurationType(std::filesystem::path filepath) {
    if (!std::filesystem::exists(filepath)) return Null;
    std::ifstream configurationFile(filepath, std::ios::binary);
    if (!configurationFile) throw std::system_error(errno, std::generic_category(), filepath.string());

    // Infer current file configuration type from first two bytes
    char buffer[3];
    if (configurationFile.read(buffer, 2).gcount() != 2) return Null;
    configurationFile.close();
    if (strncmp(buffer, "MC", 2) == 0) {
        return Mandelbrotset;
    } else if (strncmp(buffer, "TC", 2) == 0 ) {
        return Tile;
    } else if (strncmp(buffer, "PC", 2) == 0) {
        return Progress;
    }

    return Null;
}

// Returns whether or not filepath is a valid configuration based on type.
// It is okay if the file is bigger than needed, extra bytes are ignored.
bool detectConfiguration(std::filesystem::path filepath, ConfigurationType type) {
    if (!std::filesystem::exists(filepath)) return false;
    std::ifstream configurationFile(filepath, std::ios::binary);
    if (!configurationFile) throw std::system_error(errno, std::generic_category(), filepath.string());

    char configurationHeader[3];
    switch (type) {
        case Mandelbrotset: {strncpy(configurationHeader, "MC", 2);} break;
        case Tile:          {strncpy(configurationHeader, "TC", 2);} break;
        case Progress:      {strncpy(configurationHeader, "PC", 2);} break;
        default: {return false;}
    }
    char headerData[3];
    if (configurationFile.read(headerData, 2).gcount() != 2) return false;
    if (strncmp(configurationHeader, headerData, 2) != 0) {return false;}

    switch (type) {
        case Mandelbrotset: {
            uint64_t objectSize = sizeof(MandelbrotsetConfiguration);
            char objectData[objectSize];
            if (configurationFile.read(objectData, objectSize).gcount() != objectSize) return false;
            break;
        }
        case Tile: {
            uint64_t objectSize = sizeof(TileConfiguration);
            char objectData[objectSize];
            if (configurationFile.read(objectData, objectSize).gcount() != objectSize) return false;
            break;
        }
        case Progress: {
            uint64_t currentTile;
            configurationFile.read(reinterpret_cast<char*>(&currentTile), sizeof(currentTile));
            uint64_t tileCount;
            configurationFile.read(reinterpret_cast<char*>(&tileCount), sizeof(tileCount));
            uint64_t threadCount;
            configurationFile.read(reinterpret_cast<char*>(&threadCount), sizeof(threadCount));

            configurationFile.seekg((tileCount + 7) / 8, std::ios_base::cur);
            configurationFile.peek();
            if (!configurationFile) return false; // Check whether character at current position is available, i.e. the file is at least current position bytes big
            configurationFile.seekg((threadCount + 7) / 8, std::ios_base::cur);
            configurationFile.peek();
            if (!configurationFile) return false; // Ditto
            break;
        }
        default:
            return false;
    }

    return true;
}

// Loads filepath into the corresponding configuration variable based on type.
// There is no checking whether the variables are clean or not or even if the type is correct, just direct memory access. (DANGEROUS)
// The savefiles are not portable because savefiles from different systems might have different endianness.
void loadConfiguration(std::filesystem::path filepath, ConfigurationType type) {
    std::ifstream configurationFile(filepath, std::ios::binary);
    if (!configurationFile) throw std::system_error(errno, std::generic_category(), filepath.string());

    configurationFile.seekg(2);  // Skip 2 byte header.
    switch (type) {
        case Mandelbrotset: {
            configurationFile.read(reinterpret_cast<char*>(&mConfig), sizeof(MandelbrotsetConfiguration));
            return;
        }
        case Tile: {
            configurationFile.read(reinterpret_cast<char*>(&tConfig), sizeof(TileConfiguration));
            return;
        }
        case Progress: {
            // Read the first three uint64_t's to get array sizes for the next two variables
            configurationFile.read(reinterpret_cast<char*>(&pConfig), sizeof(ProgressConfiguration) - sizeof(size_t) * 2);

            delete[] pConfig.tileCompletion;
            char *buffer = new char[(pConfig.tileCount + 7) / 8]; // practically the same as std::ceil((float)pConfig.tileCount / 8)
            configurationFile.read(buffer, (pConfig.tileCount + 7) / 8);
            pConfig.tileCompletion = reinterpret_cast<unsigned char*>(buffer);

            delete[] pConfig.threadCompletion;
            buffer = new char[(pConfig.threadCount + 7) / 8]; // practically the same as std::ceil((float)pConfig.threadCount / 8)
            configurationFile.read(buffer, (pConfig.threadCount + 7) / 8);
            pConfig.threadCompletion = reinterpret_cast<unsigned char*>(buffer);
            return;
        }
    }
}

// Infers and loads the configuration file type and data respectively.
// Calls getConfiguration, detectConfiguration and loadConfiguration sequentially.
// Inherits properties from said functions, both good and bad.
void detectLoadConfiguration(std::filesystem::path filepath) {
    ConfigurationType type = getConfigurationType(filepath);
    if (detectConfiguration(filepath, type)) loadConfiguration(filepath, type);
}

// Saves configuration to file based on type.
// The resulting savefiles are not portable because savefiles from different systems might have different endianness.
void saveConfiguration(std::filesystem::path filepath, ConfigurationType type) {
    std::ofstream configurationFile(filepath, std::ios::binary);
    if (!configurationFile) throw std::system_error(errno, std::generic_category(), filepath.string());
    switch (type) {
        case Mandelbrotset:
            configurationFile.write("MC", 2);
            configurationFile.write(reinterpret_cast<char*>(&mConfig), sizeof(mConfig));
            return;
        case Tile:
            configurationFile.write("TC", 2);
            configurationFile.write(reinterpret_cast<char*>(&tConfig), sizeof(tConfig));
            return;
        case Progress:
            configurationFile.write("PC", 2);
            configurationFile.write(reinterpret_cast<char*>(&pConfig), sizeof(pConfig) - sizeof(size_t) * 2);
            configurationFile.write(reinterpret_cast<char*>(pConfig.tileCompletion), (pConfig.tileCount + 7) / 8);
            configurationFile.write(reinterpret_cast<char*>(pConfig.threadCompletion), (pConfig.threadCount + 7) / 8);
            return;
    }
}
