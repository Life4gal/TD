#include <helper/sprite_frame.hpp>

#include <components/core/sprite_frame.hpp>

#include <entt/entt.hpp>

namespace helper
{
	auto SpriteFrame::rect_of(entt::registry& registry, const entt::entity entity) noexcept -> sf::IntRect
	{
		using namespace components;

		assert(registry.valid(entity));
		assert(registry.all_of<sprite_frame::Frame>(entity));
		assert((registry.any_of<sprite_frame::Uniform, sprite_frame::Variable>(entity)));

		const auto [total, current] = registry.get<const sprite_frame::Frame>(entity);

		if (const auto* uniform = registry.try_get<const sprite_frame::Uniform>(entity))
		{
			return
			{
					// todo: 假定图集只有一行
					uniform->frame_position + sf::Vector2i{static_cast<int>(current * uniform->frame_size.x), 0},
					uniform->frame_size
			};
		}

		if (const auto* variable = registry.try_get<const sprite_frame::Variable>(entity))
		{
			return variable->frames[current];
		}

		std::unreachable();
	}
}
