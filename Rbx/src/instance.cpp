#include "Rbx/instance.hpp"
#include "offsets.hpp"

using namespace SB::Rbx;

std::string Instance::getName() const
{
	if (!baseAddress)
		return "";
	return **reinterpret_cast<std::string**>(baseAddress + Offsets::Rbx::Instance::name);
}

std::string SB::Rbx::Instance::getClassName() const
{
	if (!baseAddress)
		return "";
	uintptr_t classDescriptor = *reinterpret_cast<uintptr_t*>(baseAddress + Offsets::Rbx::Instance::descriptor);
	uintptr_t name = *reinterpret_cast<uintptr_t*>(classDescriptor + Offsets::Rbx::Reflection::Descriptor::name);
	return *reinterpret_cast<std::string*>(name);
}

Instance Instance::getParent() const
{
	if (!baseAddress)
		return Instance();
	return { *reinterpret_cast<uintptr_t*>(baseAddress + Offsets::Rbx::Instance::parent) };
}

std::vector<Instance> SB::Rbx::Instance::getChildren() const
{
	if (!baseAddress)
		return {};

	uintptr_t childrenVector = *reinterpret_cast<uintptr_t*>(baseAddress + Offsets::Rbx::Instance::children);
	if (!childrenVector)
		return {};

	uintptr_t first = *reinterpret_cast<uintptr_t*>(childrenVector);
	uintptr_t last = *reinterpret_cast<uintptr_t*>(childrenVector + sizeof(uintptr_t));
	size_t numChildren = ((last - first) / 16);

	std::vector<Instance> result;
	result.reserve(numChildren);
	for (size_t i = 0; i < numChildren; i++)
	{
		uintptr_t base = *reinterpret_cast<uintptr_t*>(first + i * 16);
		if (!base || *reinterpret_cast<uintptr_t*>(base + SB::Offsets::Rbx::Instance::self) != base)
			continue;
		result.push_back({ base });
	}
		
	return result;
}

std::optional<Instance> SB::Rbx::Instance::findFirstChild(const std::string& name) const
{
	const auto children = getChildren();
	for (const auto& child : children)
		if (child.getName() == name)
			return child;
	return std::nullopt;
}

std::optional<Instance> SB::Rbx::Instance::findFirstChildOfClass(const std::string& className) const
{
const auto children = getChildren();
	for (const auto& child : children)
		if (child.getClassName() == className)
			return child;
	return std::nullopt;
}
