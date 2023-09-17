#include <CrossFire.hpp>

using namespace CrossFire;

auto main() -> i32
{
	auto file = File::open("test.txt", "w");
	file->write(Slice<u8>{(u8*)"Hello, World!", 13});

	return 0;
}