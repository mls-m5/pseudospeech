// Copyright Mattias Lasersköld: mattias.l.sk@gmail.com

#include "buffer.h"
#include "settings.h"
#include "soundengine.h"
#include "speech.h"
#include "wavfile.h"
#include <chrono>
#include <cmath>
#include <iostream>
#include <sstream>
#include <thread>

using namespace std;
void silentOutput(Speech *speech, string filename) {
    std::vector<float> buffer(255);
    WavFile file(filename);

    cout << "writing to file" << endl;

    while (speech->lock) {
        speech->process(nullptr, &buffer[0], buffer.size());
        file.write(buffer);
    }
}

int main(int argc, char **argv) {
    const auto settings = Settings{argc, argv};

    Speech speech(settings.voice);

    if (settings.silent) {
        if (settings.outputFile.empty()) {
            cerr << "no outputfile specified" << endl;
            return 1;
        }
        if (settings.outputText.empty()) {
            cerr << "no text specified" << endl;
            return 1;
        }

        speech.pushText(settings.outputText);
        silentOutput(&speech, settings.outputFile);

        cout << "end of silent output";
        return 0;
    }

    SoundEngine::Init("speech", settings.outputDevice);

    if (!settings.outputFile.empty()) {
        SoundEngine::SetOutputFile(settings.outputFile);
        cout << "Saving to file: " << settings.outputFile << endl;
    }
    else if (!settings.noGreeting && settings.outputText.empty()) {
        speech.pushText("välkommen");
    }

    speech.writeBack(settings.writeBack);

    SoundEngine::AddElement(&speech);
    SoundEngine::Activate();
    cout << "SpeechSynth" << endl;

    if (settings.outputText.empty()) {
        string line;
        while (cin) {
            getline(cin, line);
            speech.pushText(line);
        }
    }
    else {
        speech.pushText(settings.outputText);
        while (speech.lock) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            // Spin
        }
    }
    cout << "stänger av..." << endl;
    SoundEngine::Close();
}
