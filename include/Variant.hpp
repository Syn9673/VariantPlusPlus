#pragma once
#include <variant>
#include <vector>
#include <string>

enum class VariantArgTypes {
	NONE   = 0x0,
	FLOAT1 = 0x1,
	STRING = 0x2,
	FLOAT2 = 0x3,
	FLOAT3 = 0x4,
	UINT   = 0x5,
	INT    = 0x9
};

class Variant {
private:
	uint8_t argCount;
	std::vector<unsigned char> bytes;
	std::vector <std::variant<std::string, long long, std::vector<float>>> args;

	template <int expectedSize>
	void append(long long val) {
		auto size = bytes.size();
		bytes.resize(size + expectedSize);

		auto data = bytes.data();
		memcpy(data + size, &val, expectedSize);
	}

	template <int expectedSize>
	void append(float val) {
		auto size = bytes.size();
		bytes.resize(size + expectedSize);

		auto data = bytes.data();
		memcpy(data + size, &val, expectedSize);
	}

public:
	Variant() {
		argCount = 0;
		bytes.push_back(argCount);
		args.clear();
	}

	~Variant() {
		bytes.clear();
		argCount = 0;
	}

	void push(std::variant<std::string, long long, std::vector<float>> arg) {
		const size_t type = arg.index();
		args.push_back(arg);

		switch (type) {
			case 0x0: { // strings
				auto val = std::get<std::string>(arg);
				auto size = val.length();

				bytes.push_back(argCount);
				bytes.push_back(0x2); // type

				append<sizeof(int)>((long long)size);
				for (int i = 0; i < size; i++) {
					bytes.push_back(val.at(i));
				}
				break;
			}

			case 0x1: { // long
				auto val = std::get<long long>(arg);

				bytes.push_back(argCount);
				bytes.push_back(val < 0 ? 0x9 : 0x5);

				append<sizeof(int)>(val);
				break;
			}

			case 0x2: { // float
				auto val = std::get<std::vector<float>>(arg);
				auto size = val.size();
				auto type = 0;

				switch (size) {
					case 0x1: {
						type = 0x1;
						break;
					}

					case 0x2: {
						type = 0x3;
						break;
					}

					case 0x3: {
						type = 0x4;
						break;
					}
				}

				bytes.push_back(argCount);
				bytes.push_back(type);

				for (int i = 0; i < size; ++i) {
					append<sizeof(float)>(val.at(i));
				}

				break;
			}
		}

		argCount++;
	}

	Variant& operator<<(std::variant<std::string, long long, std::vector<float>> arg) {
		push(arg);
		return *this;
	}

	Variant& operator+(std::variant<std::string, long long, std::vector<float>> arg) {
		push(arg);
		return *this;
	}

	void operator>>(unsigned char** data) {
		*data = bytes.data();
	}

	void operator=(unsigned char** data) {
		*data = bytes.data();
	}

	template <typename T>
	T& at(int index) {
		return std::get<T>(args.at(index));
	}

	VariantArgTypes type(int index) {
		auto val = args.at(index);

		if (std::get_if<std::string>(&val)) return VariantArgTypes::STRING;
		else if (auto v = std::get_if<long long>(&val)) {
			if (*v < 0) return VariantArgTypes::INT;
			else return VariantArgTypes::UINT;
		} else {
			auto floats = std::get<std::vector<float>>(val);
			VariantArgTypes type = VariantArgTypes::NONE;

			switch (floats.size()) {
				case 1: {
					type = VariantArgTypes::FLOAT1;
					break;
				}

				case 2: {
					type = VariantArgTypes::FLOAT2;
					break;
				}

				case 3: {
					type = VariantArgTypes::FLOAT3;
					break;
				}
			}

			return type;
		}
	}

	uint8_t& count() {
		return argCount;
	}

	auto size() {
		return bytes.size();
	}
};