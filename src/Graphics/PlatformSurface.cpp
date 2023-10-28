#include  <Graphics/PlatformSurface.hpp>

#if PLATFORM_WINDOWS
#include <Graphics/Surface/WindowsSurface.hpp>
#endif

namespace CrossFire
{

auto PlatformSurface::create(const char *title, int width, int height)
    -> Result<SharedPtr<PlatformSurface>, PlatformSurfaceError>
{

    return PlatformSurfaceError::API_NOT_SUPPORTED;
}

} // namespace CrossFire