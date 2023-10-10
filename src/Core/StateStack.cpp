#include <Core/StateStack.hpp>

namespace CrossFire
{

StateStack::StateStack(Allocator &allocator)
    : states(allocator)
{
    PROFILE_ZONE;
}

auto StateStack::push(State *state) -> void
{
    PROFILE_ZONE;
    states.push(state);
}

auto StateStack::pop() -> void
{
    PROFILE_ZONE;
    states.pop();
}

auto StateStack::fixed_update(AppEvent &event) -> void
{
    PROFILE_ZONE;
    if (states.data.len > 0)
        states.back()->fixed_update(event);
}

auto StateStack::update(AppEvent &event) -> void
{
    PROFILE_ZONE;
    if (states.data.len > 0)
        states.back()->update(event);
}

auto StateStack::render(AppEvent &event) -> void
{
    PROFILE_ZONE;
    if (states.data.len > 0)
        states.back()->render(event);
}

auto StateStack::clear() -> void
{
    PROFILE_ZONE;
    states.clear();
}

}