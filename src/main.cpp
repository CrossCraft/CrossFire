#include <CrossFire.hpp>
using namespace CrossFire;

class MyState : public State {
public:
    MyState()
    {
        PROFILE_ZONE;
    }

    auto init() -> void override
    {
        PROFILE_ZONE;
    }

    auto fixed_update(AppEvent &event) -> void override
    {
        PROFILE_ZONE;
        (void)event;
    }

    auto update(AppEvent &event) -> void override
    {
        PROFILE_ZONE;
        (void)event;
    }

    auto render(AppEvent &event) -> void override
    {
        PROFILE_ZONE;
        (void)event;
    }

    auto deinit() -> void override
    {
        PROFILE_ZONE;
    }
};

class MyApplication : public Application {
public:
    explicit MyApplication(Allocator &allocator)
        : Application(allocator)
    {
        PROFILE_ZONE;
        Logger::get_stdout().info("MyApplication created!");
        Logger::get_stdout().flush();
    }

    auto init() -> void override
    {
        PROFILE_ZONE;
        Logger::get_stdout().info("MyApplication initialized!");
        Logger::get_stdout().flush();

        m_state_stack.push(stack_allocator.create<MyState>().unwrap());
    }

    auto deinit() -> void override
    {
        PROFILE_ZONE;
        Logger::get_stdout().info("MyApplication deinitialized!");
        Logger::get_stdout().flush();
    }
};

auto main() -> i32
{
    PROFILE_ATTACH;
    PROFILE_ZONE;
    PROFILE_MESSAGE("Program started.");

    auto gpa = GPAllocator(256 * 1000 * 1000);
    auto allocator = DebugAllocator(gpa);

    auto app = MyApplication(allocator);
    app.run();

    return EXIT_SUCCESS;
}