#include "ShipRoute.h"

#include <utility>

ShipRoute::ShipRoute() {
    _pos = 0;
    _route = vector<string>();
}

ShipRoute::ShipRoute(vector<string> route): _pos(0), _route(std::move(route)) {}

vector<string> ShipRoute::getRoute() const{
    vector<string> remainingRoute(_route.begin() + _pos, _route.end());
    return remainingRoute;
}

bool ShipRoute::portInRoute(const string& port_symbol) {
    for(auto it = _route.begin() + _pos; it != _route.end(); ++it) {
        if(*it == port_symbol) {
            return true;
        }
    }
    return false;
}

string ShipRoute::getCurrentPort() {
    return _route[_pos];
}


void ShipRoute::next() {
    _pos += 1;
}

bool ShipRoute::isLastStop() {
    return static_cast<long unsigned int>(_pos) == _route.size() -1;
}
