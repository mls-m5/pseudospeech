
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
		fname = "samples/" + letter + ".wav";
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

			string singleCharacterConsonants = "bcdfghkljmnpqrstvz";

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
				if (time < 0) {
					//Wait
				}
				else if (time >= length) {
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

		if (time > length) {
			finished = true;
		}
	}

	void setPause(double length) {
		this->length = length;
		noiseValue = 0;
		toneValue = 0;
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
	bool consonant = false;

	BufferPtr buffer;
};

class Speech: public Element {
public:
	Speech() {
		vowel.finished = true;
		consonant.finished = true;
	}

	void process(sample_t* in, sample_t* out, int bufferSize) override {

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

//			letter.process(out, bufferSize);

//			if (letter.finished) {
//				_queue.pop();
//			}
		}

		if (not vowel.finished) {
			vowel.process(out, bufferSize);
		}

		if (not consonant.finished) {
			consonant.process(out, bufferSize);
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

	void pushText(string text) {
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
	Letter vowel;
	Letter consonant;
};



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
