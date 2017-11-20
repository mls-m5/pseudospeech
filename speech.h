//Copyright Mattias Lasersköld mattias.l.sk@gmail.com

#include "element.h"
#include <memory>
#include <queue>
#include "buffer.h"
#include "util.h"
#include <map>
#include <string>
#include <atomic>

typedef std::shared_ptr<Buffer> BufferPtr;



class Letter {
public:
	Letter() {
	}

	Letter(std::string letter, class Speech& parent);

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
	std::string name;

	BufferPtr buffer;
};



class Speech: public Element {
public:
	Speech(std::string voice = "");
	

	void process(sample_t* in, sample_t* out, int bufferSize) override;

	//Check if queue is empty
	bool isEmpty() {
		return _queue.empty();
	}
	
	//Check if there is no more sound produced
	bool dryOutput() {
		return isEmpty() && vowel.finished && consonant.finished;
	}

	void pushLetter(Letter letter) {
		_queue.push(letter);
	}

	void pushLetter(std::string letterStr) {
		Letter letter(letterStr, *this);
		pushLetter(letter);
	}
	
	void writeBack(bool state) {
		_writeBack = state;
	}
	
	bool writeBack() {
		return _writeBack;
	}

	void pushText(std::string text) {
		lock = true;
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
	std::string voice;
	bool _writeBack = false;
	bool lock = false;
	
protected:
	void loadCharacters(std::string voice);
	
	
void loadLetter(std::string letter, std::string voice = "", std::string fname = "");

	std::map<std::string, BufferPtr> bufferMap;
	
	friend Letter;

};

