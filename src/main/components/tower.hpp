#pragma once

#include <entt/entity/fwd.hpp>

#include <SFML/Graphics/CircleShape.hpp>

namespace components::tower
{
	// 武器配置
	class Weapon
	{
	public:
		using fire_type = auto(*)(entt::registry& registry, entt::entity attacker, entt::entity victim) noexcept -> void;

		float range;
		float fire_rate;

		fire_type on_fire;
	};

	// 武器冷却(如果处于冷却状态)
	class Cooldown
	{
	public:
		float delay;
	};

	// 武器目标(如果有目标)
	class Target
	{
	public:
		entt::entity entity;
	};

	// 绘制塔
	class Render
	{
	public:
		sf::CircleShape shape;
	};

	// 绘制塔与目标连线
	class DebugRenderTarget
	{
	public:
		sf::VertexArray lines;
	};
}
