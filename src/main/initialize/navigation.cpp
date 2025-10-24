#include <initialize/navigation.hpp>

#include <components/map/map.hpp>
#include <components/map/navigation.hpp>

#include <entt/entt.hpp>

namespace initialize
{
	auto navigation(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		const auto& [tile_map] = registry.ctx().get<const map_ex::TileMap>();
		const auto& [start_gates] = registry.ctx().get<const map_ex::StartGate>();
		const auto& [end_gates] = registry.ctx().get<const map_ex::EndGate>();

		map::FlowField flow_field{tile_map};
		{
			flow_field.build(end_gates);
		}

		std::vector<map::path_type> cache_paths{};
		{
			cache_paths.reserve(start_gates.size());

			// 为每个起点到最佳终点计算路径
			for (const auto start: start_gates)
			{
				auto path = flow_field.path_of(start, std::numeric_limits<std::size_t>::max());
				assert(path.has_value());

				cache_paths.emplace_back(std::move(*path));
			}
		}

		registry.ctx().emplace<navigation::FlowField>(std::move(flow_field));
		registry.ctx().emplace<navigation::Path>(std::move(cache_paths));
	}
}
