#pragma once

#include "../PlatformSurface.hpp"

#if PLATFORM_WINDOWS || PLATFORM_POSIX
#include <GLFW/glfw3.h>

namespace CrossFire
{

struct GLFWSurfaceContext {
    GLFWwindow *window;
    const char *title;
    u32 width;
    u32 height;
};

struct GLFWSurface : public PlatformSurface {
    GLFWSurface(const char *title, u32 width, u32 height);
    ~GLFWSurface() override = default;

    auto init(GraphicsAPI api) -> ResultVoid<PlatformSurfaceError> override;
    auto deinit() -> void override;

    auto update() -> void override;
    auto render() -> void override;

    auto close_request() -> bool override;

    [[nodiscard]] auto get_width() const -> u32 override;
    [[nodiscard]] auto get_height() const -> u32 override;

    [[nodiscard]] auto get_title() const -> const char * override;
};

}
#endif
