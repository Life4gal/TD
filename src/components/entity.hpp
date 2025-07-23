#pragma once

#include <utility>
#include <limits>

namespace components
{
	class EntityType
	{
	public:
		enum class Type : std::uint32_t {};

		constexpr static auto invalid_type = static_cast<Type>(std::numeric_limits<std::underlying_type_t<Type>>::max());

		Type type;
	};
}
