// Copyright Mattias Lasersköld: mattias.l.sk@gmail.com

#include "buffer.h"
#include "soundengine.h"
#include "speech.h"
#include "wavfile.h"
#include <chrono>
#include <cmath>
#include <iostream>
#include <sstream>
#include <thread>

using namespace std;

string helptext = R"xx(
a speech syntheziser
usage:
speech [options]

-v [voice]              select voice eg matt, rob, r2
-o [file]               output to file
-t                      read the remainding arguments as text 
                        (use this as last argument)
--nohello               disable the wellcome message
-s                      silent, print directly to file
--writeback             print the spoken text (good for learning eg r2 sounds)
-d [device number]      select sound output device
-h / --help             print this text

examples:
Use rob voice:
./speech -v rob

Use r2 voice, enable writeback:
./speech -v r2 --writeback


)xx";

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
    struct {
        string voice = "mat";
        int outputDevice = -1;
        string outputFile = "";
        bool noGreeting = false;
        bool writeBack = false;
        bool silent = false;
        string outputText;
    } settings;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];

        if (arg == "-v") {
            ++i;
            settings.voice = argv[i];
        }
        else if (arg == "-d") {
            ++i;

            istringstream ss(argv[i]);
            ss >> settings.outputDevice;
            cout << "selecting outputdevice " << settings.outputDevice << endl;
        }
        else if (arg == "-o") {
            ++i;
            settings.outputFile = argv[i];
        }
        else if (arg == "--nohello") {
            settings.noGreeting = true;
        }
        else if (arg == "--writeback") {
            settings.writeBack = true;
        }
        else if (arg == "-h" || arg == "--help") {
            cout << helptext << endl;
            return 0;
        }
        else if (arg == "-t") {
            for (int j = i + 1; j < argc; ++j) {
                settings.outputText += (argv[j] + string(" "));
            }
            cout << "reading text: " << endl;
            cout << settings.outputText << endl;
        }
        else if (arg == "-s") {
            settings.silent = true;
        }
    }

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
    auto sampleRate = SoundEngine::GetSampleRate();

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
