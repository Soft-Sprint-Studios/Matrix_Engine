/*
 * This file contains the interface for the WAVConverter class used for audio
 * file conversion to 44.1 kHz, stereo, and 16-bit PCM format in order to convert wavs to usuable wavs for Matrix Engine.
 */

#include "wavconverter.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_directory> <output_directory>" << std::endl;
        return 1;
    }

    std::string inputDir = argv[1];
    std::string outputDir = argv[2];

    WAVConverter converter;
    if (converter.convert(inputDir, outputDir)) {
        std::cout << "Conversion completed successfully." << std::endl;
        return 0;
    }
    else {
        std::cerr << "Conversion failed." << std::endl;
        return 1;
    }
}