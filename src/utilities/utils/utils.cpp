#include "utils.h"
#include <roblox/security/obfuscation/number.h>
#include <roblox/security/obfuscation/string.h>

//#include <cryptopp/base64.h>
//#include <cryptopp/cryptlib.h>

namespace module {
	utils* utils::singleton{ nullptr };
	auto utils::getSingleton() -> utils* {
		if (singleton == nullptr)
			singleton = new utils();

		return singleton;
	}

	auto utils::randomInteger(std::intptr_t min, std::intptr_t max) -> std::intptr_t {
		std::random_device rd;
		std::mt19937 eng(rd());
		std::uniform_int_distribution<std::intptr_t> distr(min, max);
		return distr(eng);
	};

	auto utils::randomString(std::intptr_t length) -> std::string {
		const auto randchar = [this]() -> char {
			std::string charSet = obfuscate_str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
			const auto maxIndex = charSet.size() - 1;
			return charSet.at(randomInteger(1, 200000) % maxIndex);
		};

		std::string str(length, 0);
		std::generate_n(str.begin(), length, randchar);
		return str;
	};


	auto utils::splitString(std::string str, std::string splitter, std::vector<std::string>& tokens) -> void {
		tokens.push_back(str);
		const auto splitLen = splitter.size();
		while (true) {
			auto frag = tokens.back();
			const auto splitAt = frag.find(splitter);

			if (splitAt == std::string::npos)
				break;

			tokens.back() = frag.substr(0, splitAt);
			tokens.push_back(frag.substr(splitAt + splitLen, frag.size() - (splitAt + splitLen)));
		}
	};

	auto utils::replaceAll(std::string& str, const std::string& from, const std::string& to) -> std::string {
		if (from.empty())
			return "";

		size_t startPos = 0;
		while ((startPos = str.find(from, startPos)) != std::string::npos) {
			str.replace(startPos, from.length(), to);
			startPos += to.length();
		}

		return str;
	};

	auto utils::strRepeat(std::string str, std::intptr_t length) -> std::string {
		std::stringstream ss;

		for (std::intptr_t i = 0; i < length; i++)
			ss << str;

		return ss.str();
	};

	auto utils::base64Encode(const unsigned char* data, size_t inLen) -> std::string {
		std::string encoded = "";

		/*CryptoPP::StringSource ss(data, inLen, true,
			new CryptoPP::Base64Encoder(
				new CryptoPP::StringSink(encoded)
			));*/

		return encoded;
	};

	auto utils::base64Decode(const std::string& data) -> std::string {
		std::string decoded = "";
		/*CryptoPP::StringSource ss(data, true,
			new CryptoPP::Base64Decoder(
				new CryptoPP::StringSink(decoded)
			));*/

		return decoded;
	};

	auto utils::rebaseAddress(std::intptr_t address, std::intptr_t base) -> std::intptr_t {
		static std::intptr_t moduleBase = reinterpret_cast<std::intptr_t>(GetModuleHandle(NULL));
		return ((address - base) + moduleBase);
	};

	auto utils::unbaseAddress(std::intptr_t address, std::intptr_t base) -> std::intptr_t {
		static std::intptr_t moduleBase = reinterpret_cast<std::intptr_t>(GetModuleHandle(NULL));
		return ((address - moduleBase) - base);
	};

	auto utils::crashRoblox() -> bool {
		((DWORD(__cdecl*)())nullptr)();

		for (;;) {
			exit(0);
			quick_exit(0);
			ExitProcess(0);
			_Exit(0);
			_exit(0);
		}

		return true; /* clear eax */
	};
}