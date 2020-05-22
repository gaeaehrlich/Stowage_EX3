#include "WeightBalanceCalculator.h"

int WeightBalanceCalculator::readShipPlan(const std::string& full_path_and_file_name) {
    (void)full_path_and_file_name;
    return 0;
}

WeightBalanceCalculator::BalanceStatus WeightBalanceCalculator::tryOperation(char loadUnload, int kg, int x, int y) {
    (void)loadUnload; (void)kg; (void)x; (void)y;
    return APPROVED;
}
