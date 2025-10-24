#include <initialize/map.hpp>

#include <components/game/asset.hpp>
#include <components/map/map.hpp>

#include <helper/asset.hpp>

#include <entt/entt.hpp>

namespace initialize
{
	auto map(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		const auto& texture = helper::Asset::texture_of(registry, asset::constants::map);
		registry.ctx().emplace<map_ex::Background>(sf::Sprite{texture});

		// todo: 现在将起点/终点视为地图的一部分,也就是说起点/终点已经位于地图上,无需额外绘制
		// 是否支持一张地图但是起点/终点位于不同位置?
	}
}
