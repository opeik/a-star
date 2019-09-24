#include "Pathing.hpp"

std::array<GridLocation, 4> SquareGrid::DIRS = {GridLocation{1, 0},
    GridLocation{0, -1}, GridLocation{-1, 0}, GridLocation{0, 1}};

bool operator==(GridLocation a, GridLocation b) {
    return a.x == b.x && a.y == b.y;
}

bool operator!=(GridLocation a, GridLocation b) {
    return !(a == b);
}

bool operator<(GridLocation a, GridLocation b) {
    return std::tie(a.x, a.y) < std::tie(b.x, b.y);
}

std::ostream &operator<<(std::ostream &out, const GridLocation &loc) {
    out << '(' << loc.x << ',' << loc.y << ')';
    return out;
}

void add_wall(SquareGrid &grid, int x1, int y1, int x2, int y2) {
    for (auto x = x1; x <= x2; x++) {
        for (auto y = y1; y <= y2; y++) {
            grid.walls.insert(GridLocation{x, y});
        }
    }
}

double heuristic(GridLocation a, GridLocation b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}
