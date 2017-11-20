
#pragma once

#include <string>
#include <fstream>
#include <vector>



class WavFile {
public:
    WavFile() {}

    WavFile(std::string filename, int channels = 1) {
        open(filename, channels);
    }
    ~WavFile() {
        close();
    }
    
    void open(std::string filename, int channels = 1);
    void write(float *values, int size);
    void write(std::vector<float> &data) {
        write(&data[0], data.size());
    }
    void close();


    operator bool() {
        return (bool) file;
    }

    std::ofstream file;
    size_t data_chunk_pos = 0;
    size_t file_size = 0;
    int channels = 1;
};


