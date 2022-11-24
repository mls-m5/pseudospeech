
/*
 * buffer.cpp
 *
 *  Created on: 31 jan 2014
 *      Author: mattias
 */

#include "buffer.h"
#include "sndfile.hh"
#include "soundengine.h"

std::map<int, Buffer *> Buffer::bank;

bool Buffer::read(std::string const fname, int channel) {
    SndfileHandle file;

    file = SndfileHandle(fname.c_str());
    if (file.error()) {
        printf("Could not open file %s\n", fname.c_str());
        return true;
    }

    //	printf ("Opened file '%s'\n", fname.c_str()) ;
    //	printf ("    Sample rate : %d\n", file.samplerate ()) ;
    //	printf ("    Channels    : %d\n", file.channels ()) ;
    //	printf ("    Samples    : %ld\n", file.frames () / file.channels()) ;

    std::vector<float> tmpData;
    tmpData.resize(file.frames() * file.channels());
    file.read(&tmpData.front(), tmpData.size());

    resize(file.frames());

    for (int i = 0; i < file.frames(); ++i) {
        at(i) = tmpData[i * file.channels() + channel];
    }

    puts("");

    _source = fname;
    _channels = 1;
    return false;
}

bool Buffer::write(const std::string fname) {
    SndfileHandle file;

    if (_channels == 0) {
        throw std::exception();
    }

    printf("Writing sound to file named '%s'\n", fname.c_str());
    printf("    Sample rate : %d\n", _sampleRate);
    printf("    Channels    : %d\n", _channels);
    printf("    Samples    : %ld\n", size());

    const auto format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    file =
        SndfileHandle(fname.c_str(), SFM_WRITE, format, _channels, _sampleRate);

    file.write(&at(0), size());

    puts("");
}

Buffer::Buffer() {}

Buffer::Buffer(std::string fname) {
    read(fname);
}

sample_t Buffer::interpolate(double t) {
    auto tt = (int)t;
    if (tt < 0)
        return front();
    if (tt > size())
        return back();
    auto r1 = operator[](tt);
    auto r2 = operator[](tt + 1);
    auto dt = t - tt;
    return r1 + (r2 - r1) * dt;
}

Buffer::Buffer(int length)
    : Buffer() {
    _channels = 1;
    _sampleRate = SoundEngine::GetSampleRate();
    Resize(length);
}

void Buffer::silence() {
    for (auto &it : *this) {
        it = 0;
    }
}

Buffer *Buffer::Bank(int id) {
    auto it = bank.find(id);
    if (it == bank.end()) {
        return 0;
    }
    else {
        return it->second;
    }
}

Buffer *Buffer::LoadBank(int id, std::string fname) {
    auto t = Bank(id);
    auto buffer = new Buffer(fname);
    if (buffer->empty()) {
        delete buffer;
        if (t) {
            return t;
        }
        else {
            return 0;
        }
    }
    else {
        if (t) {
            delete t;
        }
        bank[id] = buffer;
        return buffer;
    }
}
