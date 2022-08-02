#include "typedefs.hpp"

std::ostream &operator<<(std::ostream &ost, const Point &p) {
    ost << '(' << p.x << ',' << p.y << ')';
    return ost;
}
