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

JaccardData compareImagesJaccard(const QImage &im1, const QImage &im2) {
    static const QColor colors[] = { Qt::black, Qt::red, Qt::yellow, Qt::white };
    if (im1.width() != im2.width() || im1.height() != im2.height()) return {};

    JaccardData ret{ QImage(im1.size(), im1.format()) };

    for (int y = 0; y < im1.height(); ++y) {
        for (int x = 0; x < im1.width(); ++x) {
            const bool w1 = (im1.pixelColor(x, y) == Qt::white), w2 = (im2.pixelColor(x, y) == Qt::white);
            ret.disj += (w1 || w2);
            ret.conj += (w1 && w2);
            (w1 == w2 ? ret.conc : ret.disc)++;

            ret.compImage.setPixelColor(x, y, colors[w1 | (w2 << 1)]);
        }
    }

    if (ret.disj != 0) ret.coeff = 1.0 * ret.conj / ret.disj;

    return ret;
}
