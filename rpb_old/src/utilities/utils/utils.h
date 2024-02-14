#pragma once
#include <Windows.h>
#include <sstream>
#include <cstdint>
#include <string>
#include <vector>
#include <random>

//#pragma region cryptopp includes
//#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
//#include <cryptopp/aes.h>
//#include <cryptopp/rsa.h>
//#include <cryptopp/gcm.h>
//#include <cryptopp/eax.h>
//#include <cryptopp/md2.h>
//#include <cryptopp/md5.h>
//#include <cryptopp/sha.h>
//#include <cryptopp/sha3.h>
//#include <cryptopp/osrng.h>
//#include <cryptopp/hex.h>
//#include <cryptopp/pssr.h>
//#include <cryptopp/base64.h>
//#include <cryptopp/modes.h>
//#include <cryptopp/filters.h>
//#include <cryptopp/serpent.h>
//#include <cryptopp/pwdbased.h>
//#include <cryptopp/blowfish.h>
//#include <cryptopp/modes.h>
//#pragma endregion

namespace module {
	class utils {
		static utils* singleton;

	public:
		static auto getSingleton() -> utils*;

		auto randomInteger(std::intptr_t min, std::intptr_t max) -> std::intptr_t;
		auto randomString(std::intptr_t length) -> std::string;

		auto splitString(std::string str, std::string splitter, std::vector<std::string>& tokens) -> void;
		auto replaceAll(std::string& str, const std::string& from, const std::string& to) -> std::string;
		auto strRepeat(std::string str, std::intptr_t length) -> std::string;

		auto base64Encode(const unsigned char* data, size_t inLen) -> std::string;
		auto base64Decode(const std::string& data) -> std::string;

		auto rebaseAddress(std::intptr_t address, std::intptr_t base = 0x400000) -> std::intptr_t;
		auto unbaseAddress(std::intptr_t address, std::intptr_t base = 0x000000) -> std::intptr_t;

		/*template<typename T>
		static __forceinline auto hashWithAlgo(const std::string& input) -> std::string {
			T hash;
			std::string digest;

			CryptoPP::StringSource ss(input, true,
				new CryptoPP::HashFilter(hash,
					new CryptoPP::HexEncoder(
						new CryptoPP::StringSink(digest), false
					)
				)
			);

			return digest;
		};*/

		template<typename T = std::intptr_t>
		auto numberToString(T num) -> std::string {
			std::stringstream stream;
			stream << num;
			return stream.str();
		};

		template<typename T = std::intptr_t>
		auto numberToHex(T num) -> std::string {
			std::stringstream stream;
			stream
				<< "0x"
				<< std::hex << num;
			return stream.str();
		};

		auto crashRoblox() -> bool;
	};
}