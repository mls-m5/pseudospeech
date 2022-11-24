/*
 * common.h
 *
 *  Created on: 6 mar 2015
 *      Author: Mattias Larsson Sköld
 */

#pragma once

#ifdef __ANDROID__

typedef float sample_t;
#define _POSIX_SOURCE // Prevent redefinition of variables in alsa headers

#include <android/log.h>
#define debug_print(...)                                                       \
    __android_log_print(ANDROID_LOG_INFO, "synth", __VA_ARGS__)
#define debug_write(...)                                                       \
    __android_log_write(ANDROID_LOG_INFO, "synth", __VA_ARGS__)

#else

#ifdef __WIN32__

#define sample_t float

#else

// #include <jack/jack.h>
#define sample_t float // jack_default_audio_sample_t

#endif // win32-check

#include <stdio.h> //printf

#define debug_print(...) printf(__VA_ARGS__)
#define debug_write(...) printf(__VA_ARGS__)

#endif // android-check

constexpr double pi = 3.1415926535897932384626433832795028841971693;
constexpr double pi2 = pi * 2;
constexpr float pif = pi;
constexpr float pi2f = pi2;
const double referenceAFrequency = 440;
const double referenceATone = 69;

inline double roundDown(double value, double factor = 1.) {
    return factor * (double)((int)(value / factor));
}
