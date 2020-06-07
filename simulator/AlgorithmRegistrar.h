#ifndef STOWAGE_ALGORITHMREGISTRAR_H
#define STOWAGE_ALGORITHMREGISTRAR_H

#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <regex>
#include <iostream>
#include <dlfcn.h>
#include <filesystem>
#include <fstream>
#include "../algorithm/AbstractAlgorithm.h"

using std::vector;
using std::string;
using std::pair;
using std::unique_ptr;
namespace fs = std::filesystem;

class AlgorithmRegistrar {
    struct DlCloser {
        void operator()(void *dlhandle) const noexcept {
            int ret = dlclose(dlhandle);
            // TODO: erase - debbugind prints
            const char* dlcloseError = dlerror();
            const char* error = dlcloseError ? dlcloseError : "none";
            if(ret != 0) std::cout << "DLCLOSE ERROR: " << error << std::endl;
        }
    };

    vector<std::function<unique_ptr<AbstractAlgorithm>()>> _algorithmFactory;
    vector<unique_ptr<void, DlCloser>> _handles;
    vector<string> _names;

    AlgorithmRegistrar() = default;
    ~AlgorithmRegistrar();
    void registerAlgorithm(std::function<unique_ptr<AbstractAlgorithm>()> algorithmFactory);

public:
    friend struct AlgorithmRegistration;

    vector<pair<string, unique_ptr<AbstractAlgorithm>>> getAlgorithms() const;
    size_t size() const;
    static AlgorithmRegistrar& getInstance();
    void loadAlgorithmFromFile(const string& dirPath, const string& errorPath);
};


#endif //STOWAGE_ALGORITHMREGISTRAR_H
