#ifndef STOWAGE_WEIGHTBALANCECALCULATOR_H
#define STOWAGE_WEIGHTBALANCECALCULATOR_H

#include <string>

#pragma once

class WeightBalanceCalculator {
public:

    enum BalanceStatus {
        APPROVED, X_IMBALANCED, Y_IMBALANCED, X_Y_IMBALANCED
    };

    int readShipPlan(const std::string& full_path_and_file_name);

    BalanceStatus tryOperation(char loadUnload, int kg, int x, int y);
};


#endif //STOWAGE_WEIGHTBALANCECALCULATOR_H
