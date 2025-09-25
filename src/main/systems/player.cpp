#include <systems/player.hpp>

#include <components/map.hpp>
#include <components/player.hpp>

#include <components/tags.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace systems
{
	auto Player::update(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		auto& [player_resource] = registry.ctx().get<player::Resource>();
		auto& player_health = player_resource[resource::Type::HEALTH];

		// 到达终点的敌人
		const auto enemy_reached_view = registry.view<tags::cod_reached>();
		const auto reached_count = enemy_reached_view.size();

		// todo: 每个敌人扣多少生命值?
		if (const auto cost_health = static_cast<resource::size_type>(reached_count) * 1;
			cost_health >= player_health)
		{
			player_health = 0;

			// todo: 游戏结束
		}
		else
		{
			player_health -= cost_health;
		}
	}

	auto Player::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		using namespace components;

		// 绘制游标方框
		{
			const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

			auto& [cursor] = registry.ctx().get<player::Cursor>();

			const auto mouse_position = sf::Mouse::getPosition(window);
			const auto mouse_grid_position = tile_map.coordinate_world_to_grid(sf::Vector2f{mouse_position});

			if (tile_map.inside(mouse_grid_position.x, mouse_grid_position.y))
			{
				const auto world_position = tile_map.coordinate_grid_to_world(mouse_grid_position);
				cursor.setPosition(world_position);

				window.draw(cursor);
			}
		}
	}
}
