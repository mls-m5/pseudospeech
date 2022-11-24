/*
 * buffer.h
 *
 *  Created on: 31 jan 2014
 *      Author: mattias
 */

#pragma once

#include "common.h"
#include <map>
#include <string>
#include <vector>

class Buffer : public std::vector<sample_t> {
public:
    void Resize(int length) {
        if (length < 1)
            length = 1;
        std::vector<sample_t>::resize(length);
    }
    sample_t interpolate(double t); // TODO: interpolation method

    bool read(std::string const fname, int channel = 0);
    bool write(std::string const fname);
    void silence();
    Buffer();
    Buffer(std::string fname);
    Buffer(int length);

    static Buffer *Bank(int id);
    static Buffer *LoadBank(int id, std::string fname);

    const std::string &source() {
        return _source;
    }

    void source(const std::string fname) {
        _source = fname;
    }

protected:
    int _sampleRate = 0;
    int _channels = 0;
    static std::map<int, Buffer *> bank;
    std::string _source;
};
