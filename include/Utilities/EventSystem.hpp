#pragma once
#include "Types.hpp"

namespace CrossFire
{

struct Event {
    usize id;
    void *data;
};

typedef void (*EventSubscriber)(Event &event);

struct EventChannel {
    static constexpr usize MAX_SUBSCRIBERS = 16;
    Array<EventSubscriber, MAX_SUBSCRIBERS> subscribers;

    inline auto subscribe(EventSubscriber subscriber) -> void
    {
        for (usize i = 0; i < MAX_SUBSCRIBERS; i++) {
            if (subscribers[i] == nullptr) {
                subscribers[i] = subscriber;
                return;
            }
        }
    }

    inline auto publish(Event &event) -> void
    {
        for (usize i = 0; i < MAX_SUBSCRIBERS; i++) {
            if (subscribers[i] != nullptr) {
                subscribers[i](event);
            }
        }
    }
};

class EventSystem final {
    static constexpr usize MAX_CHANNELS = 64;
    std::map<usize, EventChannel> channels;

    EventSystem() = default;

public:
    inline static auto get() -> EventSystem &
    {
        static EventSystem instance;
        return instance;
    }

    inline auto subscribe(usize channel_id, EventSubscriber subscriber) -> void
    {
        if (channels.find(channel_id) == channels.end()) {
            channels[channel_id] = EventChannel();
        }

        channels[channel_id].subscribe(subscriber);
    }

    inline auto publish(usize channel_id, Event &event) -> void
    {
        if (channels.find(channel_id) == channels.end()) {
            return;
        }

        channels[channel_id].publish(event);
    }
};

}