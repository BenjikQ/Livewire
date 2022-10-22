#ifndef PATH_SEQUENCE_HPP
#define PATH_SEQUENCE_HPP

#include <ostream>

#include "path_item.hpp"
#include "point_item.hpp"

struct PathSequence {
    PointItem *prevPoint = nullptr;
    PathItem *prevPath = nullptr;
    PointItem *thisPoint = nullptr;
    PathItem *nextPath = nullptr;
    PointItem *nextPoint = nullptr;

    friend std::ostream &operator<<(std::ostream &ost, const PathSequence &ps) {
        ost << "\n prevPoint: " << (ps.prevPoint ? ps.prevPoint->number : -1)
            << "\n prevPath:  " << (ps.prevPath ? ps.prevPath->number : -1)
            << "\n thisPoint: " << (ps.thisPoint ? ps.thisPoint->number : -1)
            << "\n nextPath:  " << (ps.nextPath ? ps.nextPath->number : -1)
            << "\n nextPoint: " << (ps.nextPoint ? ps.nextPoint->number : -1) << '\n';

        return ost;
    }
};

#endif  // PATH_SEQUENCE_HPP
