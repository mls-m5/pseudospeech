//Copyright Mattias Lasersköld: mattias.l.sk@gmail.com

#include <iostream>
#include <cmath>
#include "soundengine.h"
#include "buffer.h"
#include "speech.h"
#include <sstream>

using namespace std;




int main(int argc, char **argv) {
	struct {
		string voice = "mat";
		int outputDevice = -1;
		string outputFile = "";
		bool noGreeting = false;
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
