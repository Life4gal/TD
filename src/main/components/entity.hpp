#pragma once

#include <limits>
#include <type_traits>
#include <string>

#include <SFML/System/Vector2.hpp>

namespace components::entity
{
	enum class Type : std::uint32_t {};

	using type_underlying_type = std::underlying_type_t<Type>;
	constexpr auto invalid_type = static_cast<Type>(std::numeric_limits<type_underlying_type>::max());

	class Name
	{
	public:
		std::string name;
	};

	class Position
	{
	public:
		sf::Vector2f position;
	};

	class Scale
	{
	public:
		sf::Vector2f scale;
	};
}
