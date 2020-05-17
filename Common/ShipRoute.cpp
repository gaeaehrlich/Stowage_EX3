#include "ShipRoute.h"

#include <utility>
#include <algorithm>
#include <iostream>

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

int ShipRoute::getPortNumber() {
    int count = 0;
    for(int i = 0; i <= _pos; i++) {
        if(_route[i] == _route[_pos]) {
            count++;
        }
    }
    return count;
}

bool ShipRoute::isStopAfter(const string& port1, const string& port2) {
    for(const string& stop: _route) {
        if(stop == port1) { return false; }
        if(stop == port2) { return true; }
    }
    return false; // shouldn't get here
}

int ShipRoute::portDistance(const string& port) {
    auto it = std::find(_route.begin() + _pos, _route.end(), port);
    return it != _route.end() ? std::distance(_route.begin() + _pos, it) : -1;
}

