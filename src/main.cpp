#include <CrossFire.hpp>
#include <iostream>
using namespace CrossFire;

auto main() -> i32
{
	auto file = FileFactory::open("test.txt", "w");
	auto writer = BufferedWriter(file.writer());

	for(auto i = 0; i < 10; i++) {
		(void)writer.raw_write(Slice<u8>::from_string("Hello World!\n"));
	}
	writer.flush();
	file.close();

	file = FileFactory::open("test.txt", "r");
	auto reader = file.reader();
	auto buffer = Slice<u8>(new u8[1024], 1024);

	auto read = reader.raw_read(buffer);
	std::cout << "Read " << read << " bytes." << std::endl;

	std::cout << "Contents:" << std::endl;
	std::cout << buffer.as_bytes().ptr << std::endl;

	return 0;
}