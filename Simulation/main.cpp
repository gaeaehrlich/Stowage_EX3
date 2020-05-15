#include <iostream>
#include <string>
#include "Simulation.h"

using std::string;

bool getPaths(int argc, char *argv[], string& travel_path, string& algorithm_path, string& output_path) {
    for(int i = 1; i < argc; i++) {
        if(strncmp(argv[i], "-travel_path", strlen(argv[i])) == 0) {
            if(i + 1 < argc) {
                travel_path = argv[i + 1];
            }
            else {
                std::cout << "Error: Missing path for travel_path, even though declared" << std::endl;
                return false;
            }
        }
        if(strncmp(argv[i], "-algorithm_path", strlen(argv[i])) == 0) {
            if(i + 1 < argc) {
                algorithm_path = argv[i + 1];
            }
            else {
                std::cout << "Error: Missing path for algorithm_path, even though declared" << std::endl;
                return false;
            }
        }
        if(strncmp(argv[i], "-output", strlen(argv[i])) == 0) {
            if(i + 1 < argc) {
                output_path = argv[i + 1];
            }
            else {
                std::cout << "Error: Missing path for output, even though declared" << std::endl;
                return false;
            }
        }
    }
    if(travel_path.empty()) {
        std::cout << "FATAL ERROR: Missing path for travel_path" << std::endl;
        return false;
    }
    return true;
}

int main(int argc, char *argv[]) {
    string travel_path, algorithm_path = "." + SUBDIR, output_path = "." + SUBDIR;
    if(!getPaths(argc, argv, travel_path, algorithm_path, output_path)) {
        return FAILURE;
    }
    std::cout << "Hello, World!" << std::endl;
    Simulation simulation;
    simulation.start(travel_path, algorithm_path, output_path);
    return 0;
}
