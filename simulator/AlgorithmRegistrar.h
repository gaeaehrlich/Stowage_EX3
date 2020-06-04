#ifndef STOWAGE_ALGORITHMREGISTRAR_H
#define STOWAGE_ALGORITHMREGISTRAR_H

#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <regex>
#include <dlfcn.h>
#include <experimental/filesystem>
#include <fstream>
#include "../algorithm/AbstractAlgorithm.h"

using std::vector;
using std::string;
using std::pair;
using std::unique_ptr;
namespace fs = std::experimental::filesystem;

class AlgorithmRegistrar {
    struct DlCloser {
        void operator()(void *dlhandle) const noexcept {
            dlclose(dlhandle);
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
