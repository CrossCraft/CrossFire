#pragma once

namespace CrossFire
{

struct AppEvent;

struct State {
    State() = default;
    virtual ~State() = default;

    virtual auto init() -> void = 0;

    virtual auto fixed_update(AppEvent &event) -> void = 0;
    virtual auto update(AppEvent &event) -> void = 0;
    virtual auto render(AppEvent &event) -> void = 0;

    virtual auto deinit() -> void = 0;
};

}