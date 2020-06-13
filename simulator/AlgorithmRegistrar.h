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
#include <experimental/filesystem>
#include <fstream>
#include "../algorithm/AbstractAlgorithm.h"

using std::vector;
using std::string;
using std::pair;
using std::unique_ptr;
using std::map;
namespace fs = std::experimental::filesystem;

class AlgorithmRegistrar {
    struct DlCloser {
        void operator()(void *dlhandle) const noexcept {
            dlclose(dlhandle);
        }
    };

    map<string, std::function<unique_ptr<AbstractAlgorithm>()>> _algorithmFactory;
    vector<unique_ptr<void, DlCloser>> _handles;
    vector<string> _names;
    string _currAlgName;

    AlgorithmRegistrar() = default;
    ~AlgorithmRegistrar();
    void registerAlgorithm(std::function<unique_ptr<AbstractAlgorithm>()> algorithmFactory);

public:
    friend struct AlgorithmRegistration;

    size_t size() const;
    static AlgorithmRegistrar& getInstance();
    vector<pair<string, string>> loadAlgorithmFromFile(vector<pair<string, string>> algPath);
    map<string, std::function<unique_ptr<AbstractAlgorithm>()>> getAlgorithmFactory() const;
};


#endif //STOWAGE_ALGORITHMREGISTRAR_H
