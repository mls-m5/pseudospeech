#pragma once

#include <iostream>
#include <sstream>
#include <string>

std::string helptext = R"xx(
a speech syntheziser
usage:
speech [options]

-v [voice]              select voice eg matt, rob, r2
-o [file]               output to file
-t                      read the remainding arguments as text
                        (use this as last argument)
--nohello               disable the wellcome message
-s                      silent, print directly to file
--writeback             print the spoken text (good for learning eg r2 sounds)
-d [device number]      select sound output device
-h / --help             print this text

examples:
Use rob voice:
./speech -v rob

Use r2 voice, enable writeback:
./speech -v r2 --writeback


)xx";

struct Settings {
    std::string voice = "mat";
    int outputDevice = -1;
    std::string outputFile = "";
    bool noGreeting = false;
    bool writeBack = false;
    bool silent = false;
    std::string outputText;

    Settings(int argc, char **argv) {

        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];

            if (arg == "-v") {
                ++i;
                voice = argv[i];
            }
            else if (arg == "-d") {
                ++i;

                std::istringstream ss(argv[i]);
                ss >> outputDevice;
                std::cout << "selecting outputdevice " << outputDevice
                          << std::endl;
            }
            else if (arg == "-o") {
                ++i;
                outputFile = argv[i];
            }
            else if (arg == "--nohello") {
                noGreeting = true;
            }
            else if (arg == "--writeback") {
                writeBack = true;
            }
            else if (arg == "-h" || arg == "--help") {
                std::cout << helptext << std::endl;
                std::exit(0);
            }
            else if (arg == "-t") {
                for (int j = i + 1; j < argc; ++j) {
                    outputText += (argv[j] + std::string(" "));
                }
                std::cout << "reading text: " << std::endl;
                std::cout << outputText << std::endl;
            }
            else if (arg == "-s") {
                silent = true;
            }
        }
    }
};
