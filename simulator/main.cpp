#include <iostream>
#include <string>
#include "Simulation.h"

using std::string;

bool getArgs(int argc, char **argv, string& travelPath, string& algorithmPath, string& outputPath, int& numThreads) {
    for(int i = 1; i < argc; i++) {
        if(strncmp(argv[i], "-travel_path", strlen(argv[i])) == 0) {
            if(i + 1 < argc) {
                travelPath = argv[i + 1];
            }
        }
        if(strncmp(argv[i], "-algorithm_path", strlen(argv[i])) == 0) {
            if(i + 1 < argc) {
                algorithmPath = argv[i + 1];
            }
        }
        if(strncmp(argv[i], "-output", strlen(argv[i])) == 0) {
            if(i + 1 < argc) {
                outputPath = argv[i + 1];
            }
        }
        if(strncmp(argv[i], "-num_threads", strlen(argv[i])) == 0) {
            if(i + 1 < argc) {
                numThreads = std::stoi(argv[i + 1]);
            }
        }
    }
    if(travelPath.empty()) {
        std::cout << "FATAL ERROR: Missing path for travelPath" << std::endl;
        return false;
    }
    return true;
}

bool checkDirectories(const string &travelPath, string &algorithmPath, string &outputPath) {
    bool travel = Reader::checkDirPath(travelPath);
    bool algorithm = Reader::checkDirPath(algorithmPath);
    bool output = Reader::checkDirPath(outputPath);
    if(!travel) {
        std::cout << "The path for the travels is incorrect: " << travelPath << ". simulator terminated" << std::endl;
    }
    if(!algorithm) {
        std::cout << "The path for the algorithms is incorrect: " << algorithmPath << ". simulator will use current working directory instead" << std::endl;
        algorithmPath = ".";
    }
    if(!output) {
        std::cout << "The path for the output is incorrect: " << outputPath << ". simulator will use current working directory instead" << std::endl;
        outputPath = ".";
    }
    return travel;
}

int main(int argc, char *argv[]) {
    string travelPath, algorithmPath = "." + SUBDIR, outputPath = "." + SUBDIR;
    int numThreads = 1;
    if(!getArgs(argc, argv, travelPath, algorithmPath, outputPath, numThreads) || !checkDirectories(travelPath, algorithmPath, outputPath) || numThreads < 1) {
        return FAILURE;
    }
    std::cout << "Hello, World!" << std::endl;
    Simulation simulation(numThreads);
    simulation.start(travelPath, algorithmPath, outputPath, numThreads);
    return 0;
}
