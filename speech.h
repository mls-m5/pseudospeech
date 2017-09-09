//Copyright Mattias Lasersköld mattias.l.sk@gmail.com

#include "element.h"
#include <memory>
#include <queue>
#include "buffer.h"
#include "util.h"
#include <map>
#include <string>

typedef std::shared_ptr<Buffer> BufferPtr;

extern std::map<std::string, BufferPtr> bufferMap;

class Letter {
public:
	Letter() {

	}

	Letter(std::string letter);

	void process(sample_t *out, int bufferSize);

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

	void process(sample_t* in, sample_t* out, int bufferSize) override;

	bool isEmpty() {
		return _queue.empty();
	}

	void pushLetter(Letter letter) {
		_queue.push(letter);
	}

	void pushLetter(std::string letterStr) {
		Letter letter(letterStr);
		pushLetter(letter);
	}

	void pushText(std::string text) {
		std::string str;
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

	std::queue<Letter> _queue;
	Letter vowel;
	Letter consonant;
};

