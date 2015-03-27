
#include <iostream>
#include <cmath>
#include <queue>
#include <memory>
#include "soundengine.h"
#include "buffer.h"

using namespace std;

typedef shared_ptr<Buffer> BufferPtr;
map<string, BufferPtr> bufferMap;


bool isUtfTail(char c) {
	return((c & 0xC0) == 0x80);
}

double rnd() {
	constexpr double max2 = RAND_MAX * 2.;
	return (double) rand() / max2 - 1.;
}

double triangle(double t) {
	return t - (int) t - .5;

}

double lowPass(double sample, double history, double cutof) {
	return sample * cutof + history * (1 - cutof);
}

void loadLetter(string letter, string fname = "") {
	if (fname.empty()) {
		fname = letter + ".wav";
	}
	BufferPtr buffer(new Buffer(fname));
	bufferMap[letter] = buffer;
}

class Letter {
public:
	Letter() {

	}

	Letter(std::string letter) {
		if (not letter.empty()) {
			buffer = bufferMap[letter];
			if (buffer) {
				startFrequency = stopFrequency = 1;
				length = buffer->size();
			}
			else {
				length = 0;
			}
		}
	}

	void process(sample_t *out, int bufferSize) {
		if (buffer) {
			for (int i = 0; i < bufferSize; ++i) {
				time += startFrequency;
				if (time >= length) {
					out[i] = 0;
				}
				else {
					out[i] = buffer->at(time);
				}
			}
		}
		else {
			for (auto i = 0; i < bufferSize; ++i) {
				double frequency = startFrequency;
				double step = (double) 1. / SoundEngine::GetSampleRate() * frequency;
				time += step;
				if (time > length) {
					out[i] = 0;
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
					out[i] = rnd() * noise + triangle(time) * tone;
				}
			}
		}

		if (time > length) {
			finished = true;
		}
	}

	double length = .1;
	double time = 0;
	bool finished = false;

	double noiseValue = 1;
	double toneValue = 1;
	bool toneDrop = false;
	bool fadeOut = false;
	double startFrequency = 440.;
	double stopFrequency = 880.;

	BufferPtr buffer;
};

class Speech: public Element {
public:
	void process(sample_t* in, sample_t* out, int bufferSize) override {
		if (!isEmpty()) {
			auto &letter = _queue.front();
			letter.process(out, bufferSize);

			if (letter.finished) {
				_queue.pop();
			}
		}
		else {
			for (int i = 0; i < bufferSize; ++i) {
				out[i] = 0;
			}
		}
	}

	bool isEmpty() {
		return _queue.empty();
	}

	void pushLetter(Letter letter) {
		_queue.push(letter);
	}

	void pushLetter(string letterStr) {
		Letter letter(letterStr);
		pushLetter(letter);
	}

	void pushString(string text) {
		string str;
		text += " "; //to flush the output
		for (auto c: text) {
			c = tolower(c);
			if (isUtfTail(c)) {
				str += c;
			}
			else {
				if (not str.empty()) {
					pushLetter(str);
				}
				str.clear();
				str += c;
			}
		}
	}

	queue<Letter> _queue;
};



int main() {
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
//	for (int i = 0; i < 5; ++i) {
//		{
//			Letter letter;
//			letter.noiseValue = 0;
//			speech.pushLetter(letter);
//		}
//		{
//			Letter letter;
//			letter.toneValue = 0;
//			letter.fadeOut = true;
//			speech.pushLetter(letter);
//		}
//		{
//			Letter letter;
//			letter.toneValue = 0;
//			speech.pushLetter(letter);
//		}
//	}

	speech.pushString("välkommen");

	SoundEngine::AddElement(&speech);
	SoundEngine::Activate();
	cout << "SpeechSynth" << endl;

	string line;
	while (cin) {
		getline(cin, line);
		speech.pushString(line);
	}
	cout << "stänger av..." << endl;
	SoundEngine::Close();
}
