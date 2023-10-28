#include  <Graphics/PlatformSurface.hpp>

namespace CrossFire
{

auto PlatformSurface::create(const char *title, int width, int height)
    -> Result<SharedPtr<PlatformSurface>, PlatformSurfaceError>
{
    (void)title;
    (void)width;
    (void)height;
    return PlatformSurfaceError::API_NOT_SUPPORTED;
}

} // namespace CrossFire