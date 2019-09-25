#include "Pathing.hpp"

using std::array;
using std::vector;

using namespace Pathing;

auto Location::operator==(const Location &rhs) const -> bool {
    return this->x == rhs.x && this->y == rhs.y;
}

auto Location::operator!=(const Location &rhs) const -> bool {
    return !(*this == rhs);
}

auto Location::operator<(const Location &rhs) const -> bool {
    return std::tie(this->x, this->y) < std::tie(rhs.x, rhs.y);
}

auto Grid::addWall(int x1, int y1, int x2, int y2) -> void {
    for (auto x = x1; x <= x2; x++) {
        for (auto y = y1; y <= y2; y++) {
            this->walls.insert(Location{x, y});
        }
    }
}

auto Grid::inBounds(Location id) const -> bool {
    return 0 <= id.x && id.x < width && 0 <= id.y && id.y < height;
}

auto Grid::passable(Location id) const -> bool {
    return walls.find(id) == walls.end();
}

auto Grid::neighbors(Location id) const -> vector<Location> {
    auto results = vector<Location>{};

    for (auto dir : DIRS) {
        auto next = Location{id.x + dir.x, id.y + dir.y};

        if (inBounds(next) && passable(next)) {
            results.push_back(next);
        }
    }

    // Aesthetic improvement on square grids.
    if ((id.x + id.y) % 2 == 0) {
        std::reverse(results.begin(), results.end());
    }

    return results;
}

auto WeightedGrid::cost([[maybe_unused]] Location from, Location to) const
    -> double {
    return forests.find(to) != forests.end() ? 5 : 1;
}

auto AStar::findPath(const WeightedGrid &grid, Location start, Location goal)
    -> vector<Location> {
    auto frontier = ReversePriorityQueue<Location, double>{};
    frontier.put(start, 0);

    this->cameFrom[start]  = start;
    this->costSoFar[start] = 0;

    while (!frontier.empty()) {
        auto current = frontier.get();

        if (current == goal) {
            break;
        }

        for (auto next : grid.neighbors(current)) {
            auto newCost = this->costSoFar[current] + grid.cost(current, next);

            if (this->costSoFar.find(next) == this->costSoFar.end() ||
                newCost < this->costSoFar[next]) {
                this->costSoFar[next] = newCost;
                auto priority         = newCost + heuristic(next, goal);
                frontier.put(next, priority);
                this->cameFrom[next] = current;
            }
        }
    }

    auto path = this->reconstructPath(start, goal);

    return path;
}

auto AStar::reconstructPath(Location start, Location goal) -> vector<Location> {
    auto path    = vector<Location>{};
    auto current = goal;

    while (current != start) {
        path.push_back(current);
        current = this->cameFrom[current];
    }

    path.push_back(start); // Optional.
    std::reverse(path.begin(), path.end());

    return path;
}

auto AStar::heuristic(Location a, Location b) -> double {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}
