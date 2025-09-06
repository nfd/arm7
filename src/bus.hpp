#pragma once

#include <optional>
#include <stdexcept>
#include <tuple>

template<typename... Attachments>
class Bus {
private:
	std::tuple<Attachments...> attachments;

	template<typename Width>
	requires (std::is_same_v<Width, uint8_t> || std::is_same_v<Width, uint16_t> || std::is_same_v<Width, uint32_t>)
	auto read(uint32_t address) -> Width {
		std::optional<Width> result;

		std::apply([address, &result](auto&&... attachment) {
			((attachment.contains(address) && (result = attachment.template read<Width>(address), true)) || ...);
		}, attachments);

		if (result) return *result;
		throw std::out_of_range("Address not mapped in any attachment");
	}

	template<typename Width>
	requires (std::is_same_v<Width, uint8_t> || std::is_same_v<Width, uint16_t> || std::is_same_v<Width, uint32_t>)
	void write(uint32_t address, Width value) {
		bool written = false;
		std::apply([address,value,&written](auto&&... attachment) {
			((attachment.contains(address) && (attachment.write(address, value), written = true)) || ...);
		}, attachments);

		if (!written) throw std::out_of_range("Address not mapped in any attachment");
	}

public:
	Bus(Attachments&... atts) : attachments(atts...) {}

	uint32_t read32(uint32_t address) {
		return read<uint32_t>(address);
	}

	uint16_t read16(uint32_t address) {
		return read<uint16_t>(address);
	}

	uint8_t read8(uint32_t address) {
		return read<uint8_t>(address);
	}

	void write32(uint32_t address, uint32_t value) {
		write<uint32_t>(address, value);
	}

	void write16(uint32_t address, uint16_t value) {
		write<uint16_t>(address, value);
	}

	void write8(uint32_t address, uint8_t value) {
		write<uint8_t>(address, value);
	}
};
