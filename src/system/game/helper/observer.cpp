#include <system/game/helper/observer.hpp>

#include <component/game/observer.hpp>
#include <component/game/map.hpp>

#include <entt/entt.hpp>

namespace
{
	enum class Archetype : std::uint8_t
	{
		GROUND = 0b0000'0001,
		AERIAL = 0b0000'0010,
		DUAL = GROUND | AERIAL,
	};

	template<Archetype A>
	[[nodiscard]] constexpr auto do_query(entt::registry& registry, const sf::Vector2f world_position, const float range) noexcept -> std::vector<entt::entity>
	{
		using namespace game::component;

		using size_type = std::make_signed_t<map::TileMap::size_type>;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

		const auto& [enemies_ground, enemies_aerial] = registry.ctx().get<const observer::EnemyArchetype>();

		const auto grid_position = tile_map.coordinate_world_to_grid(world_position);
		const auto horizontal_cells = static_cast<size_type>(std::ceilf(range / static_cast<float>(tile_map.tile_width())));
		const auto vertical_cells = static_cast<size_type>(std::ceilf(range / static_cast<float>(tile_map.tile_height())));

		std::vector<entt::entity> result{};

		for (size_type y = -vertical_cells; y <= vertical_cells; ++y)
		{
			for (size_type x = -horizontal_cells; x <= horizontal_cells; ++x)
			{
				const auto grid_signed = sf::Vector2i{grid_position} + sf::Vector2i{x, y};

				if (not tile_map.inside(grid_signed.x, grid_signed.y))
				{
					continue;
				}

				const auto grid_unsigned = sf::Vector2u{grid_signed};

				if constexpr (std::to_underlying(A) & std::to_underlying(Archetype::GROUND))
				{
					if (const auto it = enemies_ground.find(grid_unsigned);
						it != enemies_ground.end())
					{
						result.append_range(it->second);
					}
				}

				if constexpr (std::to_underlying(A) & std::to_underlying(Archetype::AERIAL))
				{
					if (const auto it = enemies_aerial.find(grid_unsigned);
						it != enemies_aerial.end())
					{
						result.append_range(it->second);
					}
				}
			}
		}

		return result;
	}
}

namespace game::system::helper
{
	auto Observer::query_ground(entt::registry& registry, const sf::Vector2f world_position, const float range) noexcept -> std::vector<entt::entity>
	{
		return do_query<Archetype::GROUND>(registry, world_position, range);
	}

	auto Observer::query_aerial(entt::registry& registry, const sf::Vector2f world_position, const float range) noexcept -> std::vector<entt::entity>
	{
		return do_query<Archetype::AERIAL>(registry, world_position, range);
	}

	auto Observer::query_dual(entt::registry& registry, const sf::Vector2f world_position, const float range) noexcept -> std::vector<entt::entity>
	{
		return do_query<Archetype::DUAL>(registry, world_position, range);
	}
}
