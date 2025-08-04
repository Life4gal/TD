#pragma once

#include <utility>
#include <limits>

#include <SFML/System/Vector2.hpp>

namespace game::component::entity
{
	enum class Type : std::uint32_t {};

	using type_underlying_type = std::underlying_type_t<Type>;
	constexpr auto invalid_type = static_cast<Type>(std::numeric_limits<type_underlying_type>::max());

	class Position
	{
	public:
		sf::Vector2f position;
	};
}
