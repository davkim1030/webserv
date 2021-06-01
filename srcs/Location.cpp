#include "Location.hpp"

Location::Location() {}

Location::Location(Location const &lo)
{
    option = lo.option;
}

Location &Location::operator=(Location const &lo)
{
    if (this != &lo)
        option = lo.option;
    return *this;
}

Location::~Location() {}
