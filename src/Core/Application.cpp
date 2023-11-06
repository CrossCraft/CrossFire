#include <Core/Application.hpp>
#if PLATFORM_WINDOWS || PLATFORM_POSIX
#include <Graphics/Surface/GLFWSurface.hpp>
#endif

namespace CrossFire
{

Application *Application::s_instance = nullptr;

isize Application::TARGET_FIXED_UPS = 20;
isize Application::TARGET_UPS = 144;
isize Application::TARGET_FPS = -1;

Application::Application(Allocator &allocator)
    : m_state_stack(allocator)
    , m_surface(nullptr)
{
    PROFILE_ZONE;
    cf_assert(!s_instance, "Application already exists!");
    s_instance = this;
    Logger::get_stdout().info("Application created!");
}

auto Application::run(const ApplicationSettings &settings) -> void
{
    PROFILE_ZONE;
    // Create surface

#if PLATFORM_WINDOWS || PLATFORM_POSIX
    m_surface =
        stack_allocator.create<GLFWSurface>("CrossFire", 1280, 720).unwrap();
    m_surface->init(settings.graphics_api);
#else
    cf_assert(false, "No surface implementation for this platform!");
#endif

    // Call initialization functions.
    init();

    // Setup state stack in event handler
    EventSystem::get().subscribe(
        static_cast<usize>(CrossFireEvent::FixedUpdate), [](Event &event) {
            // This static_cast is safe because we know that the event data is an AppData reference.
            get()->m_state_stack.fixed_update(static_cast<AppEvent &>(event));
        });

    EventSystem::get().subscribe(
        static_cast<usize>(CrossFireEvent::Update), [](Event &event) {
            // This static_cast is safe because we know that the event data is an AppData reference.
            get()->m_state_stack.update(static_cast<AppEvent &>(event));
        });

    EventSystem::get().subscribe(
        static_cast<usize>(CrossFireEvent::Render), [](Event &event) {
            // This static_cast is safe because we know that the event data is an AppData reference.
            get()->m_state_stack.render(static_cast<AppEvent &>(event));
        });

    // Reset timers.
    fixed_update_timer.reset();
    update_timer.reset();
    renderer_timer.reset();

    while (running) {
        // Update timers
        fixed_update_timer.update();
        update_timer.update();
        renderer_timer.update();

        // Calculate times
        auto fu_max = 1.0 / static_cast<f64>(TARGET_FIXED_UPS);
        auto u_max = 1.0 / static_cast<f64>(TARGET_UPS);
        auto r_max = 1.0 / static_cast<f64>(TARGET_FPS);

        // Get deltas
        auto fu_time = fixed_update_timer.elapsed();
        auto u_time = update_timer.elapsed();
        auto r_time = renderer_timer.elapsed();

        if (fu_time > fu_max) {
            // Fixed update
            AppData data = { this, fu_time };
            AppEvent event(CrossFireEvent::FixedUpdate, &data);
            EventSystem::get().publish(event);
            fixed_update_timer.reset();
        }

        if (u_time > u_max) {
            // Update
            m_surface->update();

            if (m_surface->close_request()) {
                running = false;
                continue;
            }

            AppData data = { this, u_time };
            AppEvent event(CrossFireEvent::Update, &data);
            EventSystem::get().publish(event);
            update_timer.reset();
        }

        if (r_time > r_max) {
            // Render
            AppData data = { this, r_time };
            AppEvent event(CrossFireEvent::Render, &data);
            EventSystem::get().publish(event);
            renderer_timer.reset();

            m_surface->render();

            running_frames += r_time;
            fps++;
            //            FrameMark;
        }

        if (running_frames > 1.0) {
            Logger::get_stdout().info(("FPS: " + std::to_string(fps)).c_str());
            Logger::get_stdout().flush();
            fps = 0;
            running_frames = 0.0;
        }
    }

    // Call cleanup functions.
    m_surface->deinit();
    deinit();
    exit(0);
}

}