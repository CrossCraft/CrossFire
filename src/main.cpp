#include <CrossFire.hpp>

using namespace CrossFire;

auto main() -> i32
{
	auto file = FileFactory::get_stdout();

	for(auto i = 0; i < 10; i++) {
		file.write(Slice<u8>::from_string("Hello, world!\n"));
	}

	return 0;
}