#pragma once

#include "Pathing.hpp"

#include <SDL.h>
#include <memory>

using Window   = std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)>;
using Renderer = std::unique_ptr<SDL_Renderer, decltype(&SDL_DestroyRenderer)>;

auto draw(Renderer &renderer, Pathing::WeightedGrid &grid,
    Pathing::Location &start) -> void;
