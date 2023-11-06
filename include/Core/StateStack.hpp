#pragma once
#include "../Utilities/EventSystem.hpp"
#include "../Utilities/List.hpp"
#include "State.hpp"

namespace CrossFire
{

struct StateStack {
    explicit StateStack(Allocator &allocator);

    Stack<State *> states;

    auto push(State *state) -> void;
    auto pop() -> void;

    auto fixed_update(AppEvent &event) -> void;
    auto update(AppEvent &event) -> void;
    auto render(AppEvent &event) -> void;

    auto clear() -> void;
};

}