#pragma once

#include <utility>
#include <limits>

#include <SFML/System/Vector2.hpp>

namespace components
{
	class EntityType
	{
	public:
		enum class Type : std::uint32_t {};

		constexpr static auto invalid_type = static_cast<Type>(std::numeric_limits<std::underlying_type_t<Type>>::max());

		Type type;
	};

	class Position
	{
	public:
		sf::Vector2f position;
	};
}
