/*
 * element.h
 *
 *  Created on: 6 mar 2015
 *      Author: Mattias Larsson Sköld
 */

#pragma once

#include "common.h"
#include <string>
#include <vector>

class Element {
public:
    Element() = default;
    virtual ~Element() = default;

    virtual void process(sample_t *in, sample_t *out, int bufferSize) = 0;
    virtual void controlSignal(){}; // hint to send controlMessages

    virtual std::string getName() {
        return elementName;
    }

    std::string elementName = "generic element";
};
