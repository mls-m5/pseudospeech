//Copyright Mattias Lasersköld: mattias.l.sk@gmail.com

#include <iostream>
#include <cmath>
#include "soundengine.h"
#include "buffer.h"
#include "speech.h"
#include <sstream>

using namespace std;



string helptext = R"xx(
a speech syntheziser
usage:
speech [options]

-v [voice]              select voice eg matt, rob, r2
-f [file]               output to file
--nohello               disable the wellcome message
--writeback             print the spoken text (good for learning eg r2 sounds)
-h                      print this text
-o [device number]      select sound output device

examples:
Use rob voice:
./speech -v rob

Use r2 voice, enable writeback:
./speech -v r2 --writeback


)xx";

int main(int argc, char **argv) {
	struct {
		string voice = "mat";
		int outputDevice = -1;
		string outputFile = "";
		bool noGreeting = false;
		bool writeBack = false;
	} settings;
	
	for (int i = 1; i < argc; ++i) {
		string arg = argv[i];
		
		if (arg == "-v") {
			++i;
			settings.voice = argv[i];
		}
		else if (arg == "-o") {
			++i;
			
			istringstream ss(argv[i]);
			ss >> settings.outputDevice;
			cout << "selecting outputdevice " << settings.outputDevice << endl;
		}
		else if (arg == "-f") {
			++i;
			settings.outputFile = argv[i];
		}
		else if(arg == "--nohello") {
			settings.noGreeting = true;
		}
		else if (arg == "--writeback") {
			settings.writeBack = true;
		}
		else if (arg == "-h" || arg == "--help") {
			cout << helptext << endl;
			return 0;
		}
		
	}

	SoundEngine::Init("speech", settings.outputDevice);



	Speech speech(settings.voice);
	
	if (!settings.outputFile.empty()) {
		SoundEngine::SetOutputFile(settings.outputFile);
		cout << "Saving to file: " << settings.outputFile << endl;
	}
	else if (! settings.noGreeting) {
		speech.pushText("välkommen");
	}
	
	speech.writeBack(settings.writeBack);
	auto sampleRate = SoundEngine::GetSampleRate();

	SoundEngine::AddElement(&speech);
	SoundEngine::Activate();
	cout << "SpeechSynth" << endl;

	string line;
	while (cin) {
		getline(cin, line);
		speech.pushText(line);
	}
	cout << "stänger av..." << endl;
	SoundEngine::Close();
}
