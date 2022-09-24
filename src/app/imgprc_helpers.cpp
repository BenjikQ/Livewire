#include "imgprc_helpers.hpp"

#include <array>
#include <queue>

std::array<QPoint, 4> near(QPoint p) {
    return {
        QPoint{ p.x() + 1, p.y() },
        QPoint{ p.x() - 1, p.y() },
        QPoint{ p.x(), p.y() + 1 },
        QPoint{ p.x(), p.y() - 1 },
    };
}

std::vector<bool> floodFill(const std::vector<bool> &region, const std::unordered_set<QPoint> &outline, QPoint origin,
                            int w, int h) {
    const auto index = [=](const QPoint &p) -> int { return p.y() * w + p.x(); };
    if (index(origin) >= region.size()) return {};
    const bool fillValue = !region[index(origin)];

    QPoint cur;
    std::queue<QPoint> to_visit{};
    std::vector<bool> affected(region.size(), false);
    to_visit.push(origin);

    while (!to_visit.empty()) {
        cur = to_visit.front();
        to_visit.pop();

        for (const auto &adj : near(cur)) {
            const auto i = index(adj);
            if (adj.x() >= 0 && adj.y() >= 0 && adj.x() < w && adj.y() < h && !affected[i] && region[i] != fillValue &&
                !outline.count(adj)) {
                to_visit.push(adj);
                affected[i] = true;
            }
        }
    }

    return affected;
}
