#pragma once
#include "Types.hpp"

namespace CrossFire
{

/**
 * @brief Event is a struct that contains an event id and data.
 */
struct Event {
    usize id;
    void *data;
};

/**
 * @brief EventSubscriber is a function pointer type for event subscribers.
 */
typedef void (*EventSubscriber)(Event &event);

/**
 * @brief EventChannel is a struct that contains a list of subscribers.
 */
struct EventChannel {
    static constexpr usize MAX_SUBSCRIBERS = 16;
    Array<EventSubscriber, MAX_SUBSCRIBERS> subscribers{};
    usize subscriber_count = 0;

    EventChannel()
    {
        PROFILE_ZONE;
        subscribers.fill(nullptr);
    }

    /**
     * @brief Subscribe to the channel.
     * @param subscriber The subscriber to add.
     */
    inline auto subscribe(EventSubscriber subscriber) -> void
    {
        PROFILE_ZONE;
        subscribers[subscriber_count++] = subscriber;
    }

    /**
     * @brief Publish an event to the channel.
     * @param event The event to publish.
     */
    inline auto publish(Event &event) -> void
    {
        PROFILE_ZONE;
        for (usize i = 0; i < MAX_SUBSCRIBERS; i++) {
            if (subscribers[i] != nullptr) {
                subscribers[i](event);
            }
        }
    }
};

/**
 * @brief EventSystem is a class that manages event channels.
 * @details This is a singleton class.
 */
class EventSystem final {
    static constexpr usize MAX_CHANNELS = 64;
    std::map<usize, EventChannel> channels;

    EventSystem() = default;

public:
    /**
     * @brief Get the EventSystem instance.
     * @return The EventSystem instance.
     */
    inline static auto get() -> EventSystem &
    {
        PROFILE_ZONE;
        static EventSystem instance;
        return instance;
    }

    /**
     * @brief Subscribe to a channel.
     * @param channel_id The channel id.
     * @param subscriber The subscriber.
     */
    inline auto subscribe(usize channel_id, EventSubscriber subscriber) -> void
    {
        PROFILE_ZONE;
        if (channels.find(channel_id) == channels.end()) {
            channels[channel_id] = EventChannel();
        }

        channels[channel_id].subscribe(subscriber);
    }

    /**
     * @brief Publish an event to a channel.
     * @param channel_id The channel id.
     * @param event The event to publish.
     */
    inline auto publish(usize channel_id, Event &event) -> void
    {
        PROFILE_ZONE;
        if (channels.find(channel_id) == channels.end()) {
            return;
        }

        channels[channel_id].publish(event);
    }
};

}