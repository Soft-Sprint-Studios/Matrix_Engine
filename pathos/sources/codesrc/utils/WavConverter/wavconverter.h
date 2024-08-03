/*
 * This file contains the interface for the WAVConverter class used for audio
 * file conversion to 44.1 kHz, stereo, and 16-bit PCM format in order to convert wavs to usuable wavs for Matrix Engine.
 */

#ifndef WAVCONVERTER_H
#define WAVCONVERTER_H

#include <string>
#include <vector>

class WAVConverter {
public:
    WAVConverter();
    ~WAVConverter();

    bool convert(const std::string& inputDir, const std::string& outputDir);

private:
    bool convertFile(const std::string& inputFilePath, const std::string& outputFilePath);
    bool ensureOutputDirExists(const std::string& outputDir);
    bool processFile(const std::string& inputFilePath, const std::string& outputFilePath);

    static const int SAMPLE_RATE = 44100;
    static const int BITS_PER_SAMPLE = 16;
    static const int CHANNELS = 2;
};

#endif // WAVCONVERTER_H
