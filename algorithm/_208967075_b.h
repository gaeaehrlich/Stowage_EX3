#ifndef STOWAGE_NAIVEALGORITHM2_H
#define STOWAGE_NAIVEALGORITHM2_H

#include "NaiveAlgorithm.h"

class _208967075_b : public NaiveAlgorithm {
    bool tryMoveFrom(unique_ptr<Container>& container, const Position old, std::ofstream &file) override;
};


#endif //STOWAGE_NAIVEALGORITHM2_H
