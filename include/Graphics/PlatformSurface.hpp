#pragma once
#include <Utilities/Types.hpp>
#include <Utilities/Allocator.hpp>
#include "APIs.hpp"

namespace CrossFire
{

enum class PlatformSurfaceError {
    API_NOT_SUPPORTED,
    WINDOW_CREATION_FAILED,
    CONTEXT_CREATION_FAILED,
    CONTEXT_INITIALIZATION_FAILED,
    ALLOCATION_FAILED
};

struct PlatformSurface {
    virtual ~PlatformSurface() = default;

    virtual auto init(GraphicsAPI api) -> ResultVoid<PlatformSurfaceError> = 0;
    virtual auto deinit() -> void = 0;

    virtual auto update() -> void = 0;
    virtual auto render() -> void = 0;

    virtual auto close_request() -> bool = 0;

    [[nodiscard]] virtual auto get_width() const -> u32 = 0;
    [[nodiscard]] virtual auto get_height() const -> u32 = 0;

    [[nodiscard]] virtual auto get_title() const -> const char * = 0;

    [[nodiscard]] auto get_context() const -> void *
    {
        return context;
    }

    void *context = nullptr;
};

}