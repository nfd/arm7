#include <algorithm>
#include <cstdint>
#include <ios>
#include <iostream>

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

static void undefined(uint32_t instruction, const char* reason) {
	std::cout << "Undefined instruction: " << std::hex << instruction << std::dec << " (" << reason << ")\n";
	throw std::runtime_error("Undefined instruction");
}

template<typename BusType>
class Cpu {
private:
	BusType& bus;

	uint32_t registers[16];
	uint32_t& pc = registers[15]; // Program Counter (R15)

	PSR cpsr;

public:
	Cpu(BusType& bus) : bus(bus) {
		std::fill_n(registers, 16, 0);
	}

	void dumpregs() {
		for(int i = 0; i < 16; i++) {
			std::cout << "R" << i << ": " << std::hex << registers[i] << std::dec << " ";
		}
		std::cout << "CPSR: N=" << cpsr.N << " Z=" << cpsr.Z << " C=" << cpsr.C << " V=" << cpsr.V
		          << " I=" << cpsr.I << " F=" << cpsr.F << " T=" << cpsr.T
		          << " Mode=" << ((cpsr.M4 << 4) | (cpsr.M3 << 3) | (cpsr.M2 << 2) | (cpsr.M1 << 1) | cpsr.M0)
		          << "\n";
	}

	void step() {
		// Fetch instruction
		uint32_t instruction = bus.read32(pc);

		pc += 4;

		if((instruction & 0xF0000000) == 0xF0000000) {
			if((instruction & 0x0F000000) == 0x0A000000) {
				/* Branch & Branch with Link and Change to Thumb */
				undefined(instruction, "Branch & Branch with Link and Change to Thumb");
			} else {
				undefined(instruction, "cond == 0xF");
			}
		}

		if(!check_condition(instruction >> 28)) {
			// Condition not met, do nothing
			return;
		}

		switch((instruction & 0x0E000000) >> 25) {
			case 0b000:
				undefined(instruction, "Data Processing, misc, multiplies");
				break;
			case 0b001:
				/* Data Processing & Miscellaneous Instructions */
				data_processing_immediate(instruction);
				break;
			case 0b010:
				load_store_immediate(instruction);
				break;
			case 0b011:
				undefined(instruction, "Load/Store register offset");
				break;
			case 0b100:
				/* Load/Store Multiple */
				undefined(instruction, "Load/Store Multiple");
				break;
			case 0b101:
				/* Branch & Branch with Link */
				{
					int32_t offset = (instruction & 0x00FFFFFF);
					if(offset & 0x00800000) {
						offset |= 0xFF000000; // Sign-extend negative offset
					}
					offset <<= 2; // Left shift by 2 to get byte offset
					pc += offset + 4; // +4 for the pipeline effect
				}
				break;
			case 0b110:
				/* Coprocessor Load/Store and Double Register Transfers */
				undefined(instruction, "Coprocessor Load/Store and Double Register Transfers");
				break;
			case 0b111:
				undefined(instruction, "Software Interrupt and Coprocessor Instructions");
				break;
			default:
				undefined(instruction, "Unknown instruction category");
		}
	}

	void data_processing_immediate(uint32_t instruction) {
		uint32_t opcode = (instruction >> 21) & 0xF;
		bool immediate = (instruction >> 25) & 1;
		if(!immediate) {
			undefined(instruction, "Data processing non-immediate?");
		}

		bool set_flags = (instruction >> 20) & 1;
		// uint32_t rn = (instruction >> 16) & 0xF;
		uint32_t rd = (instruction >> 12) & 0xF;
		uint32_t shifter_operand = instruction & 0xFFF;

		switch(opcode) {
			case 0b0010: { // SUB
				uint32_t rotate_imm = (shifter_operand >> 8) & 0xF;
				uint32_t imm8 = shifter_operand & 0xFF;
				uint32_t value = (imm8 >> (rotate_imm * 2)) | (imm8 << (32 - (rotate_imm * 2)));
				uint32_t result = registers[rd] - value;
				registers[rd] = result;
				if(set_flags) {
					cpsr.N = (result >> 31) & 1;
					cpsr.Z = (result == 0) ? 1 : 0;
					cpsr.C = (registers[rd] >= value) ? 1 : 0; // No borrow
					cpsr.V = ((registers[rd] ^ value) & (registers[rd] ^ result) >> 31) & 1;
				}
				break;
			}
			case 0b1101: { // MOV
				uint32_t rotate_imm = (shifter_operand >> 8) & 0xF;
				uint32_t imm8 = shifter_operand & 0xFF;
				uint32_t value = (imm8 >> (rotate_imm * 2)) | (imm8 << (32 - (rotate_imm * 2)));
				registers[rd] = value;
				if(set_flags) {
					cpsr.N = (value >> 31) & 1;
					cpsr.Z = (value == 0) ? 1 : 0;
				}
				break;
			}
			default:
				std::cout << "Data processing opcode not implemented: " << std::hex << opcode << std::dec << "\n";
				undefined(instruction, "Data processing opcode not implemented");
		}
	}

	void load_store_immediate(uint32_t instruction) {
		addressing_mode_2(instruction);
	}

	void addressing_mode_2(uint32_t instruction) {
		bool immediate = ((instruction >> 25) & 1) == 0;
		bool post_indexed = ((instruction >> 24) & 1) == 0;
		bool up = ((instruction >> 23) & 1) == 1;
		bool byte_access = ((instruction >> 22) & 1) == 1;
		bool write_back = ((instruction >> 21) & 1) == 1;
		bool load = ((instruction >> 20) & 1) == 1;

		if(immediate) {
			uint32_t rn = (instruction >> 16) & 0xF;
			uint32_t address = registers[rn];
			uint32_t offset_12 = instruction & 0xFFF;

			if(!post_indexed) {
				// Pre-indexed
				address = up ? address + offset_12 : address - offset_12;
			}

			if(rn == 15) {
				address += 4; // R15 is ahead by 4 bytes
			}

			uint32_t rd = (instruction >> 12) & 0xF;
			if(load) {
				// Load
				if(byte_access) {
					registers[rd] = bus.read8(address);
				} else {
					registers[rd] = bus.read32(address);
				}
			} else {
				// Store
				if(byte_access) {
					bus.write8(address, registers[rd] & 0xFF);
				} else {
					bus.write32(address, registers[rd]);
				}
			}

			if(post_indexed) {
				// Post-indexed
				address = up ? address + offset_12 : address - offset_12;
				registers[rn] = address;
			} else {
				if(write_back) {
					registers[rn] = address;
				}
			}

		} else {
			undefined(instruction, "Addressing mode 2 register?");
		}
	}

private:
	bool check_condition(uint32_t cond) {
		switch (cond) {
			case 0: return cpsr.Z; // EQ
			case 1: return !cpsr.Z; // NE
			case 2: return cpsr.C; // CS/HS
			case 3: return !cpsr.C; // CC/LO
			case 4: return cpsr.N; // MI
			case 5: return !cpsr.N; // PL
			case 6: return cpsr.V; // VS
			case 7: return !cpsr.V; // VC
			case 8: return cpsr.C && !cpsr.Z; // HI
			case 9: return !cpsr.C || cpsr.Z; // LS
			case 10: return cpsr.N == cpsr.V; // GE
			case 11: return cpsr.N != cpsr.V; // LT
			case 12: return !cpsr.Z && (cpsr.N == cpsr.V); // GT
			case 13: return cpsr.Z || (cpsr.N != cpsr.V); // LE
			case 14: return true; // AL
			default: return false; // NV (never executed)
		}
	}
};
