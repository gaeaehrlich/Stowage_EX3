#ifndef STOWAGE_NAIVEALGORITHM1_H
#define STOWAGE_NAIVEALGORITHM1_H

#include "NaiveAlgorithm.h"

class _208967075_a : public NaiveAlgorithm {
    bool tryMoveFrom(unique_ptr<Container> &container, const Position old, std::ofstream &file) override;
};

#endif //STOWAGE_NAIVEALGORITHM1_H
