#include "ram.hpp"
#include "bus.hpp"
#include "cpu.hpp"
#include "debugout.hpp"

template<uint32_t RamSize, uint32_t DebugOutAddress>
struct System {
	using RamType = Ram<0x0, RamSize>;
	using DebugOutType = DebugOut<DebugOutAddress>;
	using BusType = Bus<RamType, DebugOutType>;
	using CpuType = Cpu<BusType>;

	RamType ram;
	DebugOutType debugOut;
	BusType bus;
	CpuType cpu;

	System() : ram(), debugOut(), bus(ram, debugOut), cpu(bus) { }
};

using ConfiguredSystem = System<64 * 1024, 0x10000>;
