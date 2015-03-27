/*
 * jackengine.h
 *
 *  Created on: 13 jan 2014
 *      Author: mattias
 */

#ifndef JACKENGINE_H_
#define JACKENGINE_H_

#include <list>
#include <string>

#include "common.h"
#include "element.h"

namespace SoundEngine {
	bool Init(std::string name = "");
	bool Activate();
	bool Close();
	void AddElement(Element *e);
	void RemoveElement(Element *e);
	void SetVolume(double v);

	int GetBufferSize();
	int GetSampleRate();

#ifdef __ANDROID__
	int process (sample_t *buffer, int bufferSize);
#else
	int process (unsigned int nframes, void *arg);
#endif
};

#endif /* JACKENGINE_H_ */

