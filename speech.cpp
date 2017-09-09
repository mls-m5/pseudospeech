//Copyright Mattias Lasersköld: mattias.l.sk@gmail.com

#include "speech.h"
#include "soundengine.h"
#include <iostream>

using namespace std;

Letter::Letter(std::string letter, class Speech &parent):
name(letter) {
    if (not letter.empty()) {

        string singleCharacterConsonants = "bcdfghkljmnpqrstvz";

        buffer = parent.bufferMap[letter];
        if (buffer) {
            startFrequency = stopFrequency = 1;
            length = buffer->size();
        }
        else {
            length = 0;
        }
    }
}

void Letter::process(sample_t *out, int bufferSize) {
    if (buffer) {
        for (int i = 0; i < bufferSize; ++i) {
            time += startFrequency;
            if (time < 0) {
                //Wait
            }
            else if (time >= length) {
                finished = true;
                break;
            }
            else {
                out[i] += buffer->at(time);
            }
        }
    }
    else {
        for (auto i = 0; i < bufferSize; ++i) {
            double frequency = startFrequency;
            double step = (double) 1. / SoundEngine::GetSampleRate() * frequency;
            time += step;
            if (time > length) {
                break;
            }
            else {
                double noise = noiseValue;
                double tone = noiseValue;
                double timeAmount = (length - time) / length;
                if (fadeOut) {
                    noise = noiseValue * timeAmount;
                }
                if (toneDrop) {
                    frequency = (stopFrequency - startFrequency) * timeAmount + startFrequency;
                }
                out[i] += (rnd() * noise + triangle(time) * tone);
            }
        }
    }
}

Speech::Speech(string voice):
voice(voice) {
    vowel.finished = true;
    consonant.finished = true;
    loadCharacters(voice);
}

void Speech::loadCharacters(string voice) {
    cout << "loading voice " << voice << endl;
    string singleCharacterLetters = "abcdefghijklmnopqrstuvxyz";
	string utfCharacters[] = {"å", "ä", "ö"};
	for (auto l: singleCharacterLetters) {
		string str;
		str += l;

		loadLetter(str, voice);
	}
	for (auto l: utfCharacters) {
		loadLetter(l, voice);
    }
    

    //assuming standard sample rate
    auto sampleRate = 44100;
    bufferMap[" "] = BufferPtr(new Buffer(sampleRate / 20));
	bufferMap["."] = BufferPtr(new Buffer(sampleRate / 2));
	bufferMap[","] = bufferMap[";"] = bufferMap["."];
}


void Speech::loadLetter(string letter, string voice, string fname) {
	if (fname.empty()) {
		fname = "samples/" + voice + "/" + letter + ".wav";
	}
	BufferPtr buffer(new Buffer(fname));
	if (buffer->size() == 0) {
        cerr << "could not load " << letter << endl;
    }
    // else {
        // cout << "loaded " << letter << " with length " << buffer->size() << endl;
    // }
	bufferMap[letter] = buffer;
}


void Speech::process(sample_t* in, sample_t* out, int bufferSize) {
    for (int i = 0; i < bufferSize; ++i) {
        out[i] = 0;
    }

    if (!isEmpty()) {
        auto &letter = _queue.front();

        if (consonant.finished and vowel.finished) {
            if (not isEmpty() and _queue.front().consonant) {
                consonant = _queue.front();
                _queue.pop();
            }
        }
        if (vowel.finished) {
            if (not isEmpty() and not _queue.front().consonant) {
                vowel = _queue.front();
                if (not consonant.finished) {
                    if (vowel.buffer) {
                        vowel.time = -consonant.length;
                    }
                }
                _queue.pop();
            }
        }
    }

    if (not vowel.finished) {
        vowel.process(out, bufferSize);
    }

    if (not consonant.finished) {
        consonant.process(out, bufferSize);
    }
}
    