/*
 * headercreater.cpp
 *
 *  Created on: 10 mar 2015
 *      Author: Mattias Larsson Sköld
 */

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include <sndfile.hh>

string stripEnding(string fname) {
    for (int i = fname.size() - 1; i > 0; --i) {
        if (fname[i] == '.') {
            return fname.substr(0, i);
        }
    }
    return fname;
}

void readFile(string fname) {
    SndfileHandle file;

    file = SndfileHandle(fname.c_str());
    if (file.error()) {
        printf("Could not open file %s\n", fname.c_str());
        return;
    }

    std::vector<short> tmpData;
    tmpData.resize(file.frames() * file.channels());
    file.read(&tmpData.front(), tmpData.size());

    std::vector<short> outData;

    outData.resize(file.frames());

    for (int i = 0; i < file.frames(); ++i) {
        outData[i] = tmpData[i * file.channels()];
    }

    string endingLess = stripEnding(fname);
    ofstream outFile(endingLess + ".h");

    outFile << "short " << endingLess << " = {" << endl;
    for (auto sample : outData) {
        outFile << sample << "," << endl;
    }

    outFile << "}" << endl;
}

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        cout << "please specify sound file, followed by output file name"
             << endl;
        return 1;
    }

    cout << "reading file " << argv[1] << endl;

    readFile(argv[1]);
}
