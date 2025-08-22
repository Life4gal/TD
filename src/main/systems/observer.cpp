#include <systems/observer.hpp>

#include <components/tags.hpp>
#include <components/entity.hpp>
#include <components/map.hpp>
#include <components/observer.hpp>

#include <entt/entt.hpp>

namespace
{
	enum class Archetype : std::uint8_t
	{
		GROUND = 0b0000'0001,
		AERIAL = 0b0000'0010,
		DUAL = GROUND | AERIAL,
	};

	template<Archetype A, bool VisibleOnly>
	[[nodiscard]] constexpr auto do_query(entt::registry& registry, const sf::Vector2f world_position, const float range) noexcept -> std::vector<entt::entity>
	{
		using namespace components;

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
						if constexpr (VisibleOnly)
						{
							std::ranges::copy_if(
								it->second,
								std::back_inserter(result),
								[&](const entt::entity entity) noexcept -> bool
								{
									return not registry.all_of<tags::invisible>(entity);
								}
							);
						}
						else
						{
							result.append_range(it->second);
						}
					}
				}

				if constexpr (std::to_underlying(A) & std::to_underlying(Archetype::AERIAL))
				{
					if (const auto it = enemies_aerial.find(grid_unsigned);
						it != enemies_aerial.end())
					{
						if constexpr (VisibleOnly)
						{
							std::ranges::copy_if(
								it->second,
								std::back_inserter(result),
								[&](const entt::entity entity) noexcept -> bool
								{
									return not registry.all_of<tags::invisible>(entity);
								}
							);
						}
						else
						{
							result.append_range(it->second);
						}
					}
				}
			}
		}

		return result;
	}
}

namespace systems
{
	auto Observer::initialize(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		using size_type = observer::EnemyStatistics::size_type;

		registry.ctx().emplace<observer::EnemyArchetype>();
		registry.ctx().emplace<observer::EnemyStatistics>(size_type{0}, size_type{0});
	}

	auto Observer::update(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		using namespace components;

		using size_type = observer::EnemyStatistics::size_type;

		std::ignore = delta;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();

		auto& [enemies_ground, enemies_aerial] = registry.ctx().get<observer::EnemyArchetype>();
		auto& [ground_alive, aerial_alive] = registry.ctx().get<observer::EnemyStatistics>();

		const auto enemy_ground_view = registry.view<tags::archetype_ground, tags::enemy, const entity::Position>(entt::exclude<tags::dead>);
		const auto enemy_aerial_view = registry.view<tags::archetype_aerial, tags::enemy, const entity::Position>(entt::exclude<tags::dead>);

		ground_alive = static_cast<size_type>(enemy_ground_view.size_hint());
		aerial_alive = static_cast<size_type>(enemy_aerial_view.size_hint());

		enemies_ground.clear();
		for (const auto [entity, position]: enemy_ground_view.each())
		{
			const auto grid_position = tile_map.coordinate_world_to_grid(position.position);

			enemies_ground[grid_position].push_back(entity);
		}

		enemies_aerial.clear();
		for (const auto [entity, position]: enemy_aerial_view.each())
		{
			const auto grid_position = tile_map.coordinate_world_to_grid(position.position);

			enemies_aerial[grid_position].push_back(entity);
		}
	}

	auto Observer::query_ground(entt::registry& registry, const sf::Vector2f world_position, const float range) noexcept -> std::vector<entt::entity>
	{
		return do_query<Archetype::GROUND, false>(registry, world_position, range);
	}

	auto Observer::query_aerial(entt::registry& registry, const sf::Vector2f world_position, const float range) noexcept -> std::vector<entt::entity>
	{
		return do_query<Archetype::AERIAL, false>(registry, world_position, range);
	}

	auto Observer::query_dual(entt::registry& registry, const sf::Vector2f world_position, const float range) noexcept -> std::vector<entt::entity>
	{
		return do_query<Archetype::DUAL, false>(registry, world_position, range);
	}

	auto Observer::query_visible_ground(entt::registry& registry, const sf::Vector2f world_position, const float range) noexcept -> std::vector<entt::entity>
	{
		return do_query<Archetype::GROUND, true>(registry, world_position, range);
	}

	auto Observer::query_visible_aerial(entt::registry& registry, const sf::Vector2f world_position, const float range) noexcept -> std::vector<entt::entity>
	{
		return do_query<Archetype::AERIAL, true>(registry, world_position, range);
	}

	auto Observer::query_visible_dual(entt::registry& registry, const sf::Vector2f world_position, const float range) noexcept -> std::vector<entt::entity>
	{
		return do_query<Archetype::DUAL, true>(registry, world_position, range);
	}
}
