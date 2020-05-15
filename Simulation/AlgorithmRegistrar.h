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
#include "../Algorithm/AbstractAlgorithm.h"

using std::vector;
using std::string;
using std::unique_ptr;
namespace fs = std::experimental::filesystem;

class AlgorithmRegistrar {
    struct DlCloser {
        void operator()(void *dlhandle) const noexcept {
            dlclose(dlhandle);
        }
    };

    static AlgorithmRegistrar _instance;
    vector<std::function<unique_ptr<AbstractAlgorithm>()>> _algorithmFactory;
    vector<unique_ptr<void, DlCloser>> _handles;

    AlgorithmRegistrar() = default;
    ~AlgorithmRegistrar();
    void registerAlgorithm(std::function<unique_ptr<AbstractAlgorithm>()> algorithmFactory);

public:
    friend struct AlgorithmRegistration;

    vector<unique_ptr<AbstractAlgorithm>> getAlgorithms() const;
    size_t size() const;
    static AlgorithmRegistrar& getInstance();
    void loadAlgorithmFromFile(const string& dir_path, const string& error_path);
};


#endif //STOWAGE_ALGORITHMREGISTRAR_H
