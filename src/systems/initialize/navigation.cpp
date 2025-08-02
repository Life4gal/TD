#include <systems/initialize/navigation.hpp>

#include <components/map.hpp>
#include <components/navigation.hpp>
#include <components/render.hpp>

#include <entt/entt.hpp>

namespace systems::initialize
{
	auto navigation(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		const auto& [map] = registry.ctx().get<const map_ex::Map>();
		const auto& [start_gates, end_gates] = registry.ctx().get<const map_ex::Gate>();

		map::FlowField flow_field{map};
		{
			flow_field.build(end_gates);
		}

		std::vector<map::path_type> cache_paths{};
		{
			cache_paths.reserve(start_gates.size());

			for (const auto start_point: start_gates)
			{
				auto path = flow_field.path_of(start_point, std::numeric_limits<std::size_t>::max());
				assert(path.has_value());

				cache_paths.emplace_back(*std::move(path));
			}
		}

		// render
		{
			render::Navigation render_navigation
			{
					.path_vertices = sf::VertexArray{sf::PrimitiveType::LineStrip},
			};

			registry.ctx().emplace<render::Navigation>(std::move(render_navigation));
		}

		registry.ctx().emplace<navigation::FlowField>(std::move(flow_field));
		registry.ctx().emplace<navigation::Path>(std::move(cache_paths));
	}
}
