#ifndef STOWAGE_SHIPROUTE_H
#define STOWAGE_SHIPROUTE_H

#include <vector>
#include <string>

using std::vector;
using std::string;


class ShipRoute {
    int _pos;
    vector<string> _route;

public:
    ShipRoute();
    explicit ShipRoute(vector<string> route);
    vector<string> getRoute() const;
    bool portInRoute(const string& symbol);
    string getCurrentPort();
    void next();
    bool isLastStop();
    int getPortNumber();
    bool isStopAfter(const string& port1, const string& port2);
    int portDistance(const string& port);
    int getCurrentDistance();
    };


#endif //STOWAGE_SHIPROUTE_H
