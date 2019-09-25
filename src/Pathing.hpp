#pragma once

#include <algorithm>
#include <array>
#include <cstdlib>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace Pathing {
    struct Location {
        int x = 0;
        int y = 0;

        auto operator==(const Location &) const -> bool;
        auto operator!=(const Location &) const -> bool;
        auto operator<(const Location &) const -> bool;
    };
}

namespace std {
    template<>
    struct hash<Pathing::Location> {

        auto operator()(const Pathing::Location &e) const noexcept -> std::size_t {
            using std::hash;

            return std::hash<int>()(e.x ^ (e.y << 4));
        }
    };
}

namespace Pathing {
    struct Grid {
        static constexpr std::array<Location, 4> DIRS = {
            Location{1, 0}, Location{0, -1}, Location{-1, 0}, Location{0, 1}};

        int width  = 15;
        int height = 15;

        std::unordered_set<Location> walls = {};

        Grid(int width_, int height_) : width(width_), height(height_) {}

        auto inBounds(Location id) const -> bool;
        auto passable(Location id) const -> bool;
        auto neighbors(Location id) const -> std::vector<Location>;
        auto addWall(int x1, int y1, int x2, int y2) -> void;
    };

    struct WeightedGrid : Grid {
        std::unordered_set<Location> forests = {};

        WeightedGrid(int _width, int _height) : Grid(_width, _height) {}

        auto cost(Location from, Location to) const -> double;
    };

    template<typename T, typename Priority>
    struct ReversePriorityQueue {
        using Element = std::pair<Priority, T>;

        std::priority_queue<Element, std::vector<Element>, std::greater<Element>>
            elements = {};

        auto empty() const -> bool {
            return elements.empty();
        }

        auto put(T item, Priority priority) -> void {
            elements.emplace(priority, item);
        }

        auto get() -> T {
            auto bestItem = elements.top().second;
            elements.pop();

            return bestItem;
        }
    };

    struct AStar {
        std::unordered_map<Location, Location> cameFrom = {};
        std::unordered_map<Location, double> costSoFar  = {};

        auto findPath(const WeightedGrid &grid, Location start, Location end)
            -> std::vector<Location>;

      private:
        auto reconstructPath(Location start, Location goal)
            -> std::vector<Location>;
        auto heuristic(Location a, Location b) -> double;
    };
}
