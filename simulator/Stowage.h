#ifndef STOWAGE_STOWAGE_H
#define STOWAGE_STOWAGE_H


#include "../common/ShipPlan.h"
#include "../common/ShipRoute.h"
#include "Crane.h"

class Stowage {
public:
    ShipPlan _plan;
    ShipRoute _route;
    Crane _crane;

    Stowage getEmptyCopy() {
        Stowage stowage;
        stowage._plan = _plan.getEmptyCopy();
        stowage._route = _route;
        return std::move(stowage);
    }
};


#endif //STOWAGE_STOWAGE_H
