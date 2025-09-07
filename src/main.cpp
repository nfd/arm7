
#include "bus.hpp"
#include "ram.hpp"
#include "cpu.hpp"
#include "debugout.hpp"

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cerr << "Usage: " << argv[0] << " <executable>\n";
		return 1;
	}

	std::string executable_filename = argv[1];


	Ram<0x0, 64 * 1024> ram1; // 64KB RAM at 0x00000000
	DebugOut<0x10000> debugOut; // Debug output at 0x00010000

	Bus bus(ram1, debugOut);

	ram1.load_from_file(executable_filename, 0x0);

	Cpu cpu(bus);

	for(int i = 0; i < 100; i++) {
		cpu.step();
		cpu.dumpregs();
	}

	return 0;
}
