/*
 * soundengine-portaudio.cpp
 *
 *  Created on: 12 aug 2014
 *      Author: Mattias Larsson Sköld
 */

#include "soundengine.h"
#include "wavfile.h"
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <list>
#include <portaudio.h>
#include <stdexcept>
#include <stdio.h>
#include <unistd.h>

int SampleRate = 44100; // Temporary value
int BufferSize = 100;   // Temporary value

using namespace std;

namespace SoundEngine {
static std::list<Element *> elementList;

bool canCapture = true;
sample_t *dummyInputSample = 0;
double masterVolume = 1;
static WavFile outputFile;
static bool outputToFile = false;

} // namespace SoundEngine

static PaStream *stream = 0;

struct callbackData {
    const sample_t *in;
    sample_t *out;
    unsigned int channels;
};

// static int paCallback (void const * inputBuffer, void * outputBuffer,
// unsigned long int framesPerBuffer, PaStreamCallbackTimeInfo const * timeInfo,
// PaStreamCallbackFlags statusFlags, void * userData);

static int paCallback(void const *inputBuffer,
                      void *outputBuffer,
                      unsigned long int framesPerBuffer,
                      PaStreamCallbackTimeInfo const *timeInfo,
                      PaStreamCallbackFlags statusFlags,
                      void *userData) {
    sample_t *out = (sample_t *)outputBuffer;
    const sample_t *in = (const sample_t *)inputBuffer;

    callbackData data = {in, out, 1};

    SoundEngine::process(framesPerBuffer, (void *)&data);

    return paContinue;
}

void printInfoList() {
    int count = Pa_GetDeviceCount();

    for (int i = 0; i < count; i++) {
        const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);

        printf("Device %d: %s\n", i, deviceInfo->name);
    }
}

void SoundEngine::AddElement(Element *e) {
    elementList.push_back(e);
}

void SoundEngine::RemoveElement(Element *e) {
    elementList.remove(e);
}

int SoundEngine::process(unsigned int nframes, void *arg) {
    sample_t *out;
    auto data = (callbackData *)arg;

    if (dummyInputSample == 0) {
        // Create empty dummy buffer
        dummyInputSample = new sample_t[nframes];
    }

    auto in = dummyInputSample;

    if (canCapture) {
        for (int i = 0; i < nframes; ++i) {
            in[i] = data->in[i * data->channels];
        }
    }
    else {
        for (int i = 0; i < nframes; ++i) {
            dummyInputSample[i] = 0;
        }
    }
    out = data->out;

    bool firstElement = true;
    for (auto element : elementList) {
        if (firstElement) {
            firstElement = 0;
        }
        else {
            memcpy(in, out, sizeof(sample_t) * nframes);
        }
        element->process(in, out, nframes);
    }
    if (masterVolume != 1) {
        const auto volume = masterVolume;
        for (int i = 0; i < nframes; ++i) {
            out[i] *= volume;
        }
    }

    if (outputToFile) {
        outputFile.write(out, nframes);
    }

    for (auto element : elementList) {
        element->controlSignal();
    }
    return 0;
}

bool SoundEngine::Init(std::string name, int outputDevice) {
    if (stream)
        return 0;

    PaStreamParameters inputParameters, outputParameters;
    PaError err;
    err = Pa_Initialize();
    if (err != paNoError)
        return true;

    inputParameters.device =
        Pa_GetDefaultInputDevice(); /* default input device */
    // inputParameters.device =paNoDevice
    // inputParameters.device = 14; //OSSu

    if (inputParameters.device == paNoDevice) {
        fprintf(stderr, "Error: No default input device.\n");
        return true;
    }
    else {
        // Skriver ut information för enhet som används
        const PaDeviceInfo *deviceInfo =
            Pa_GetDeviceInfo(inputParameters.device);
        printf("Device %d: %s\n", inputParameters.device, deviceInfo->name);

        printf("Allt valbart ljud \n");
        printInfoList();
        // printf("Standard låg fördröjning: %s\n",
        // deviceInfo->defaultLowInputLatency);

        // Slut på skriver ut enhet
    }

    inputParameters.channelCount = 1; /* stereo input */
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency =
        Pa_GetDeviceInfo(inputParameters.device)->defaultHighInputLatency;
    inputParameters.hostApiSpecificStreamInfo = 0; // NULL

    if (outputDevice == -1) {
        outputParameters.device =
            Pa_GetDefaultOutputDevice(); /* default output device */
    }
    else {
        outputParameters.device = outputDevice;
        cout << "Soundengine selecting device " << outputDevice << endl;
    }

    // outputParameters.device = 14;
    if (outputParameters.device == paNoDevice) {
        fprintf(stderr, "Error: No default output device.\n");
        return true;
    }
    outputParameters.channelCount = 1; /* stereo output */
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency =
        Pa_GetDeviceInfo(outputParameters.device)->defaultHighOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = 0; // NULL

    err = Pa_OpenStream(&stream,
                        &inputParameters,
                        &outputParameters,
                        SampleRate,
                        BufferSize,
                        0,
                        /* paClipOff, */ /* we won't output out of range samples
                                            so don't bother clipping them */
                        paCallback,
                        0); // NULL
    if (err != paNoError)
        return false;

    return true;
}

void SoundEngine::SetOutputFile(std::string filename) {
    if (!filename.empty()) {
        outputFile.open(filename, 1);

        if (outputFile) {
            outputToFile = true;
        }
        else {
            cerr << "Failed to open file " << filename << endl;
        }
    }
}

bool SoundEngine::Activate() {
    PaError err;

    err = Pa_StartStream(stream);
    if (err == paNoError)
        return true;

    return false;
}

bool SoundEngine::Close() {
    printf("Finished. Shutting down portaudio");
    if (stream) {
        Pa_StopStream(stream);
        Pa_CloseStream(stream);
        stream = 0;
    }
    Pa_Terminate();

    if (dummyInputSample) {
        delete[] dummyInputSample;
    }

    outputFile.close();

    return true;
}

int SoundEngine::GetBufferSize() {
    return BufferSize;
}

int SoundEngine::GetSampleRate() {
    return SampleRate;
}

void SoundEngine::SetVolume(double v) {
    if (stream) {
        masterVolume = v;
    }
}
