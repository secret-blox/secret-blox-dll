#pragma once
#include <stdint.h>
#include <string>
#include <optional>
#include <vector>

namespace SB::Rbx {
	class Instance {
	public:
		uintptr_t baseAddress = 0;

		Instance getParent() const;

		std::string getName() const;
		std::string getClassName() const;

		std::vector<Instance> getChildren() const;
		std::optional<Instance> findFirstChild(const std::string& name) const;
		std::optional<Instance> findFirstChildOfClass(const std::string& className) const;
	};
}