#pragma once
#include "../Utilities/Types.hpp"
#include "../Utilities/Time.hpp"
#include "../Utilities/Logger.hpp"
#include "StateStack.hpp"

namespace CrossFire
{

struct AppData {
    void *app;
    double frame_time;
};

struct AppEvent : public Event {
    AppEvent(CrossFireEvent event_id, AppData *data)
        : Event(static_cast<usize>(event_id), data)
    {
    }
};

class Application {
    static Application *s_instance;

    Timer fixed_update_timer;
    Timer update_timer;
    Timer renderer_timer;

    usize fps = 0;
    f64 running_frames = 0.0;
    bool running = true;

protected:
    StateStack m_state_stack;

public:
    static isize TARGET_FIXED_UPS;
    static isize TARGET_UPS;
    static isize TARGET_FPS;

    Application(Allocator &allocator);
    virtual ~Application() = default;

    virtual auto init() -> void = 0;
    virtual auto deinit() -> void = 0;

    static auto get() -> Application *
    {
        return s_instance;
    }
    static auto quit() -> void
    {
        get()->running = false;
    }

    auto run() -> void;
};

}