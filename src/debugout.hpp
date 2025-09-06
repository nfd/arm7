#include <cstdint>
#include <iostream>
#include <stdexcept>

template <uint32_t Start>
class DebugOut {
public:
	DebugOut() = default;

	constexpr bool contains(uint32_t address, uint32_t accessSize = 1) const {
		return address == Start && accessSize <= 4;
	}

	template <typename Width>
	auto read(uint32_t address) -> Width const {
		if (!contains(address, sizeof(Width))) {
			throw std::out_of_range("Address out of range");
		}
		return 0; // Return a dummy value
	}

	template <typename Width>
	void write(uint32_t address, Width value) {
		if (!contains(address, sizeof(value))) {
			throw std::out_of_range("Address out of range");
		}
		std::cout << "Debug: " << value << "\n";
		// std::cout << value;
	}
};
