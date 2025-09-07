#pragma once

#include <cstdint>

struct PSR {
	uint32_t N : 1; // negative
	uint32_t Z : 1; // zero
	uint32_t C : 1; // carry/borrow/extend
	uint32_t V : 1; // overflow
	uint32_t reserved : 20; // Reserved bits
	uint32_t I : 1; // IRQ disable
	uint32_t F : 1; // FIQ disable
	uint32_t T : 1; // Thumb state
	uint32_t M4 : 1; // Mode bit 4
	uint32_t M3 : 1; // Mode bit 3
	uint32_t M2 : 1; // Mode bit 2
	uint32_t M1 : 1; // Mode bit 1
	uint32_t M0 : 1; // Mode bit 0

	PSR() : N(0), Z(0), C(0), V(0), reserved(0), I(0), F(0), T(0), M4(0), M3(0), M2(0), M1(0), M0(0) {}
};

template<typename BusType>
class Cpu {
private:
	BusType& bus;

	uint32_t registers[16];
	uint32_t& pc = registers[15]; // Program Counter (R15)

	PSR cpsr;
	bool halted;

public:
	Cpu(BusType& bus);
	void dumpregs();
	void step();
	bool is_halted();

private:
	void data_processing(uint32_t instruction);
	void load_store_immediate(uint32_t instruction);
	void swi_and_coprocessor(uint32_t instruction);
	void addressing_mode_2(uint32_t instruction);
	bool check_condition(uint32_t cond);
};
