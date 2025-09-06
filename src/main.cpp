#include <CLI/CLI.hpp>

#include "bus.hpp"
#include "ram.hpp"
#include "cpu.hpp"
#include "debugout.hpp"

int main(int argc, char* argv[]) {
	CLI::App app{"Arm7 Emulator?"};

	std::string executable_filename;
	app.add_option("executable", executable_filename, "Path to the program to run")->required()->check(CLI::ExistingFile);

	CLI11_PARSE(app, argc, argv);

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
