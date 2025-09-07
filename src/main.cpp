#include "config.hpp"

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <executable>\n";
		return 1;
	}

	std::string executable_filename = argv[1];

	ConfiguredSystem system;

	system.ram.load_from_file(executable_filename, 0x0);

	for(int i = 0; i < 100; i++) {
		system.cpu.step();
		system.cpu.dumpregs();
	}

	return 0;
}
