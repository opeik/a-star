#pragma once

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <queue>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

struct SimpleGraph {
    std::unordered_map<char, std::vector<char>> edges;

    std::vector<char> neighbors(char id) {
        return edges[id];
    }
};

struct GridLocation {
    int x, y;
};

namespace std {
    /* implement hash function so we can put GridLocation into an unordered_set */
    template<>
    struct hash<GridLocation> {
        typedef GridLocation argument_type;
        typedef std::size_t result_type;
        std::size_t operator()(const GridLocation &id) const noexcept {
            return std::hash<int>()(id.x ^ (id.y << 4));
        }
    };
}

struct SquareGrid {
    static std::array<GridLocation, 4> DIRS;

    int width, height;
    std::unordered_set<GridLocation> walls;

    SquareGrid(int width_, int height_) : width(width_), height(height_) {}

    bool in_bounds(GridLocation id) const {
        return 0 <= id.x && id.x < width && 0 <= id.y && id.y < height;
    }

    bool passable(GridLocation id) const {
        return walls.find(id) == walls.end();
    }

    std::vector<GridLocation> neighbors(GridLocation id) const {
        std::vector<GridLocation> results;

        for (GridLocation dir : DIRS) {
            GridLocation next{id.x + dir.x, id.y + dir.y};
            if (in_bounds(next) && passable(next)) {
                results.push_back(next);
            }
        }

        if ((id.x + id.y) % 2 == 0) {
            // aesthetic improvement on square grids
            std::reverse(results.begin(), results.end());
        }

        return results;
    }
};

struct GridWithWeights : SquareGrid {
    std::unordered_set<GridLocation> forests;
    GridWithWeights(int w, int h) : SquareGrid(w, h) {}
    double cost(GridLocation from_node, GridLocation to_node) const {
        return forests.find(to_node) != forests.end() ? 5 : 1;
    }
};

template<typename T, typename priority_t>
struct PriorityQueue {
    typedef std::pair<priority_t, T> PQElement;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> elements;

    inline bool empty() const {
        return elements.empty();
    }

    inline void put(T item, priority_t priority) {
        elements.emplace(priority, item);
    }

    T get() {
        T best_item = elements.top().second;
        elements.pop();
        return best_item;
    }
};

bool operator==(GridLocation a, GridLocation b);
bool operator!=(GridLocation a, GridLocation b);
bool operator<(GridLocation a, GridLocation b);
std::ostream &operator<<(std::ostream &out, const GridLocation &loc);

GridWithWeights make_diagram4();

template<typename Location>
std::vector<Location> reconstruct_path(Location start, Location goal,
    std::unordered_map<Location, Location> came_from) {
    std::vector<Location> path;
    Location current = goal;
    while (current != start) {
        path.push_back(current);
        current = came_from[current];
    }
    path.push_back(start); // optional
    std::reverse(path.begin(), path.end());
    return path;
}

template<class Graph>
void draw_grid(const Graph &graph, int field_width,
    std::unordered_map<GridLocation, double> *distances      = nullptr,
    std::unordered_map<GridLocation, GridLocation> *point_to = nullptr,
    std::vector<GridLocation> *path                          = nullptr) {
    for (int y = 0; y != graph.height; ++y) {
        for (int x = 0; x != graph.width; ++x) {
            GridLocation id{x, y};
            std::cout << std::left << std::setw(field_width);
            if (graph.walls.find(id) != graph.walls.end()) {
                std::cout << std::string(field_width, '#');
            } else if (point_to != nullptr && point_to->count(id)) {
                GridLocation next = (*point_to)[id];
                if (next.x == x + 1) {
                    std::cout << "> ";
                } else if (next.x == x - 1) {
                    std::cout << "< ";
                } else if (next.y == y + 1) {
                    std::cout << "v ";
                } else if (next.y == y - 1) {
                    std::cout << "^ ";
                } else {
                    std::cout << "* ";
                }
            } else if (distances != nullptr && distances->count(id)) {
                std::cout << (*distances)[id];
            } else if (path != nullptr &&
                       find(path->begin(), path->end(), id) != path->end()) {
                std::cout << '@';
            } else {
                std::cout << '.';
            }
        }
        std::cout << '\n';
    }
}

template<typename Location, typename Graph>
void a_star_search(Graph graph, Location start, Location goal,
    std::unordered_map<Location, Location> &came_from,
    std::unordered_map<Location, double> &cost_so_far) {
    PriorityQueue<Location, double> frontier;
    frontier.put(start, 0);

    came_from[start]   = start;
    cost_so_far[start] = 0;

    while (!frontier.empty()) {
        Location current = frontier.get();

        if (current == goal) {
            break;
        }

        for (Location next : graph.neighbors(current)) {
            double new_cost = cost_so_far[current] + graph.cost(current, next);
            if (cost_so_far.find(next) == cost_so_far.end() ||
                new_cost < cost_so_far[next]) {
                cost_so_far[next] = new_cost;
                double priority   = new_cost + heuristic(next, goal);
                frontier.put(next, priority);
                came_from[next] = current;
            }
        }
    }
}

double heuristic(GridLocation a, GridLocation b);
void add_wall(SquareGrid &grid, int x1, int y1, int x2, int y2);
