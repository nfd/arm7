#pragma once
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>

template<uint32_t StartAddress, uint32_t Size>
class Ram {
private:
	std::shared_ptr<uint8_t[]> data;

public:
	Ram() : data(std::make_shared<uint8_t[]>(Size)) {
		std::fill_n(data.get(), Size, 0);
	}

	constexpr bool contains(uint32_t address, uint32_t accessSize = 1) const {
		return address >= StartAddress && (address + accessSize) <= (StartAddress + Size);
	}

	template<typename Width>
	requires (std::is_same_v<Width, uint8_t> || std::is_same_v<Width, uint16_t> || std::is_same_v<Width, uint32_t>)
	auto read(uint32_t address) -> Width const {
		if (!contains(address, sizeof(Width))) {
			throw std::out_of_range("Address out of range");
		}
		Width value;
		std::memcpy(&value, &data[address - StartAddress], sizeof(Width));
		return value;
	}

	template<typename Width>
	requires (std::is_same_v<Width, uint8_t> || std::is_same_v<Width, uint16_t> || std::is_same_v<Width, uint32_t>)
	void write(uint32_t address, Width value) {
		if (!contains(address, sizeof(value))) {
			throw std::out_of_range("Address out of range");
		}
		std::memcpy(&data[address - StartAddress], &value, sizeof(value));
	}

	void load_from_file(const std::string& filename, uint32_t loadAddress) {
		if (!contains(loadAddress)) {
			throw std::out_of_range("Load address out of range");
		}
		std::ifstream file(filename, std::ios::binary);
		if (!file) {
			throw std::runtime_error("Failed to open file");
		}
		auto fileSize = file.seekg(0, std::ios::end).tellg();
		file.seekg(0, std::ios::beg);
		if(!contains(loadAddress, loadAddress + static_cast<uint32_t>(fileSize))) {
			throw std::out_of_range("File size exceeds RAM bounds");
		}

		file.read(reinterpret_cast<char*>(&data[loadAddress - StartAddress]), fileSize);
		if (!file) {
			throw std::runtime_error("Failed to read file");
		}
	}
};
