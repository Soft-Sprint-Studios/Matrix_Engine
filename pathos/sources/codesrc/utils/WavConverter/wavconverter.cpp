/*
 * This file contains the interface for the WAVConverter class used for audio
 * file conversion to 44.1 kHz, stereo, and 16-bit PCM format in order to convert wavs to usuable wavs for Matrix Engine.
 */

#include "wavconverter.h"
#include <iostream>
#include <filesystem>
#include <sndfile.h>

namespace fs = std::filesystem;

WAVConverter::WAVConverter() {}

WAVConverter::~WAVConverter() {}

bool WAVConverter::convert(const std::string& inputDir, const std::string& outputDir) {
    if (!ensureOutputDirExists(outputDir)) {
        std::cerr << "Failed to create output directory" << std::endl;
        return false;
    }

    for (const auto& entry : fs::directory_iterator(inputDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".wav") {
            std::string inputFilePath = entry.path().string();
            std::string outputFilePath = outputDir + "/" + entry.path().filename().string();

            if (!convertFile(inputFilePath, outputFilePath)) {
                std::cerr << "Failed to convert file: " << inputFilePath << std::endl;
                return false;
            }
        }
    }

    return true;
}

bool WAVConverter::convertFile(const std::string& inputFilePath, const std::string& outputFilePath) {
    return processFile(inputFilePath, outputFilePath);
}

bool WAVConverter::ensureOutputDirExists(const std::string& outputDir) {
    try {
        if (!fs::exists(outputDir)) {
            fs::create_directory(outputDir);
        }
        return true;
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        return false;
    }
}

bool WAVConverter::processFile(const std::string& inputFilePath, const std::string& outputFilePath) {
    SF_INFO sfinfo;
    SNDFILE* infile = sf_open(inputFilePath.c_str(), SFM_READ, &sfinfo);
    if (!infile) {
        std::cerr << "Error opening input file: " << sf_strerror(nullptr) << std::endl;
        return false;
    }

    SF_INFO outinfo = sfinfo;
    outinfo.samplerate = SAMPLE_RATE;
    outinfo.channels = CHANNELS;
    outinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;

    std::vector<float> buffer(sfinfo.frames * sfinfo.channels);
        sf_readf_float(infile, buffer.data(), sfinfo.frames);
    sf_close(infile);

    std::vector<int16_t> outputBuffer(buffer.size());
    float maxVal = 0.0f;

    for (float sample : buffer) {
        if (sample > maxVal) maxVal = sample;
    }

    for (size_t i = 0; i < buffer.size(); ++i) {
        outputBuffer[i] = static_cast<int16_t>(buffer[i] * 32767.0f / maxVal);
    }

    SNDFILE* outfile = sf_open(outputFilePath.c_str(), SFM_WRITE, &outinfo);
    if (!outfile) {
        std::cerr << "Error opening output file: " << sf_strerror(nullptr) << std::endl;
        return false;
    }

    sf_writef_short(outfile, outputBuffer.data(), sfinfo.frames);
    sf_close(outfile);

    return true;
}
