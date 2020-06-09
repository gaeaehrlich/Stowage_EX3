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

    // TODO create empty stowage
};


#endif //STOWAGE_STOWAGE_H
