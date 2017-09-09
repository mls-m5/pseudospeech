//Copyright Mattias Lasersköld: mattias.l.sk@gmail.com

#include <iostream>
#include <cmath>
#include <queue>
#include <memory>
#include "soundengine.h"
#include "buffer.h"
#include "speech.h"

using namespace std;

map<string, BufferPtr> bufferMap;


void loadLetter(string letter, string fname = "") {
	if (fname.empty()) {
		fname = "samples/" + letter + ".wav";
	}
	BufferPtr buffer(new Buffer(fname));
	bufferMap[letter] = buffer;
}


int main(int argc, char **argv) {
	SoundEngine::Init("speech");

	string singleCharacterLetters = "abcdefghijklmnopqrstuvxyz";
	string utfCharacters[] = {"å", "ä", "ö"};
	for (auto l: singleCharacterLetters) {
		string str;
		str += l;

		loadLetter(str);
	}
	for (auto l: utfCharacters) {
		loadLetter(l);
	}

	Speech speech;
	
	if (argc > 1) {
		SoundEngine::SetOutputFile(argv[1]);
	}
	else {
		speech.pushText("välkommen");
	}
	auto sampleRate = SoundEngine::GetSampleRate();
	bufferMap[" "] = BufferPtr(new Buffer(sampleRate / 20));
	bufferMap["."] = BufferPtr(new Buffer(sampleRate / 2));
	bufferMap[","] = bufferMap[";"] = bufferMap["."];

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
