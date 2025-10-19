#pragma once

#include <limits>
#include <type_traits>
#include <string>

#include <entt/entity/fwd.hpp>

#include <SFML/System/Vector2.hpp>

namespace components::entity
{
	enum class Type : std::uint32_t {};

	using type_underlying_type = std::underlying_type_t<Type>;

	// 占位符,用于一些和UI交互的元素
	constexpr auto invalid_type = static_cast<Type>(std::numeric_limits<type_underlying_type>::max());

	// 实体名称
	class Name
	{
	public:
		std::string name;
	};

	// 实体位置
	class Position
	{
	public:
		sf::Vector2f position;
	};

	// 实体尺寸(比例)
	class Scale
	{
	public:
		sf::Vector2f scale;
	};

	// 实体死亡回调(可以没有)
	class OnDeath
	{
	public:
		using on_death_type = auto (*)(entt::registry& registry, entt::entity entity) noexcept -> void;

		on_death_type on_death;
	};
}
