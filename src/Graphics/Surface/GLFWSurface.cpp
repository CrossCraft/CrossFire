#include <Graphics/Surface/GLFWSurface.hpp>

#if PLATFORM_WINDOWS || PLATFORM_POSIX

namespace CrossFire
{

namespace detail
{
GLFWSurfaceContext glfw_context;
}

GLFWSurface::GLFWSurface(const char *title, u32 width, u32 height)
{
    PROFILE_ZONE;
    detail::glfw_context.title = title;
    detail::glfw_context.width = width;
    detail::glfw_context.height = height;
    context = (void *)&detail::glfw_context;
}

auto GLFWSurface::init(GraphicsAPI api) -> ResultVoid<PlatformSurfaceError>
{
    PROFILE_ZONE;
    if (glfwInit() != GLFW_TRUE)
        return PlatformSurfaceError::WINDOW_CREATION_FAILED;

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

    switch (api) {
    case GraphicsAPI::OpenGL4_1:
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        break;
    case GraphicsAPI::GLES3_2:
        glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        break;
    case GraphicsAPI::Vulkan1_2:
    case GraphicsAPI::DirectX11:
    case GraphicsAPI::DirectX12:
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    detail::glfw_context.window = glfwCreateWindow(detail::glfw_context.width,
                                                   detail::glfw_context.height,
                                                   detail::glfw_context.title,
                                                   nullptr, nullptr);
    glfwSwapInterval(0);

    if (detail::glfw_context.window == nullptr)
        return PlatformSurfaceError::WINDOW_CREATION_FAILED;

    return Ok();
}
auto GLFWSurface::deinit() -> void
{
    PROFILE_ZONE;
    glfwDestroyWindow(detail::glfw_context.window);
    glfwTerminate();
}

auto GLFWSurface::update() -> void
{
    PROFILE_ZONE;
    glfwPollEvents();
}
auto GLFWSurface::render() -> void
{
    PROFILE_ZONE;
    glfwSwapBuffers(detail::glfw_context.window);
}

auto GLFWSurface::get_width() const -> u32
{
    PROFILE_ZONE;
    return detail::glfw_context.width;
}
auto GLFWSurface::get_height() const -> u32
{
    PROFILE_ZONE;
    return detail::glfw_context.height;
}

auto GLFWSurface::get_title() const -> const char *
{
    PROFILE_ZONE;
    return detail::glfw_context.title;
}
auto GLFWSurface::close_request() -> bool
{
    return glfwWindowShouldClose(detail::glfw_context.window);
}

}

#endif