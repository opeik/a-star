#include "Main.hpp"

#include "Pathing.hpp"

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <queue>
#include <stdexcept>
#include <string>
#include <utility>

using std::runtime_error;
using std::string;
using std::unique_ptr;
using std::unordered_map;
using std::vector;

using namespace Pathing;

constexpr auto SIZE    = 25;
constexpr auto START_X = 10;
constexpr auto START_Y = 10;

auto draw(Renderer &renderer, WeightedGrid &grid, Location &start) -> void {
    auto mouseX = 0;
    auto mouseY = 0;
    SDL_GetMouseState(&mouseX, &mouseY);

    auto *keys = SDL_GetKeyboardState(nullptr);

    auto gridX = std::clamp(((mouseX + START_X) / SIZE) - 1, 0, grid.width - 1);
    auto gridY = std::clamp(((mouseY + START_Y) / SIZE) - 1, 0, grid.height - 1);

    if (keys[SDL_SCANCODE_SPACE]) {
        start = Location{gridX, gridY};
    }

    // Add walls
    if (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        grid.walls.insert(Location{gridX, gridY});
    }

    // Remove walls
    if (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        for (auto i = grid.walls.begin(); i != grid.walls.end();) {
            if (i->x == gridX && i->y == gridY) {
                i = grid.walls.erase(i);
            } else {
                ++i;
            }
        }
    }

    // Draw walls
    SDL_SetRenderDrawColor(renderer.get(), 100, 100, 100, 255);
    for (auto i : grid.walls) {
        auto rect =
            SDL_Rect{START_X + i.x * SIZE, START_Y + i.y * SIZE, SIZE, SIZE};
        SDL_RenderFillRect(renderer.get(), &rect);
    }

    auto goal = Location{gridX, gridY};
    auto path = AStar{}.findPath(grid, start, goal);

    // Draw path
    SDL_SetRenderDrawColor(renderer.get(), 186, 86, 75, 255);
    for (auto i : path) {
        auto rect =
            SDL_Rect{START_X + i.x * SIZE, START_Y + i.y * SIZE, SIZE, SIZE};
        SDL_RenderFillRect(renderer.get(), &rect);
    }

    // Draw grid
    SDL_SetRenderDrawColor(renderer.get(), 255, 255, 255, 255);
    for (auto w = 0; w < grid.width; w++) {
        for (auto h = 0; h < grid.height; h++) {
            auto rect =
                SDL_Rect{START_X + w * SIZE, START_Y + h * SIZE, SIZE, SIZE};

            SDL_RenderDrawRect(renderer.get(), &rect);
        }
    }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv) {
    auto isRunning = true;

    auto grid  = WeightedGrid{25, 25};
    auto start = Location{0, 0};

    // Start SDL.
    auto status = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    if (status != 0) {
        throw runtime_error{string{"Unable to initialize SDL: "} + SDL_GetError()};
    }

    // Create window.
    auto window = Window{SDL_CreateWindow("a-star", SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, 645, 645, SDL_WINDOW_SHOWN),
        &SDL_DestroyWindow};

    if (window.get() == nullptr) {
        throw runtime_error{string{"Unable to create window: "} + SDL_GetError()};
    }

    auto renderer =
        Renderer{SDL_CreateRenderer(window.get(), -1,
                     SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
            &SDL_DestroyRenderer};

    while (isRunning) {
        auto event = SDL_Event{};

        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
        }

        SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 255);
        SDL_RenderClear(renderer.get());
        draw(renderer, grid, start);
        SDL_RenderPresent(renderer.get());
    }

    return EXIT_SUCCESS;
}
