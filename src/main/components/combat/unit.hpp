#pragma once

#include <string>

#include <entt/fwd.hpp>

namespace components::combat
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

	// 死亡回调(可选)
	class OnDeath
	{
	public:
		using on_death_type = auto (*)(entt::registry& registry, entt::entity entity) noexcept -> void;

		on_death_type on_death;
	};
}
