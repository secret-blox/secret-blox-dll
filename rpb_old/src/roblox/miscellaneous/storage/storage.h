#pragma once
#include <Windows.h>
#include <string>
#include <map>

enum storageIdx {

};

namespace module {
	class storage {
		static storage* singleton;
		std::map<storageIdx, std::uintptr_t> _items;

	public:
		using storageIterator = void(*)(storageIdx, std::uintptr_t);
	public:
		static auto getSingleton() -> storage*;

		template<typename T = std::uintptr_t>
		auto store(storageIdx index, T value) -> void {
			this->_items[index] = reinterpret_cast<std::uintptr_t>(value);
		}

		template<typename T = std::uintptr_t>
		auto find(storageIdx index) -> T {
			return reinterpret_cast<T>(this->_items[index]);
		}

		auto exist(storageIdx index) -> bool;
		auto foreach(storageIterator it) -> void;
		auto remove(storageIdx index) -> void;
	};
}