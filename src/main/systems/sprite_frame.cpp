#include <systems/sprite_frame.hpp>

#include <components/sprite_frame.hpp>
#include <components/renderable.hpp>

#include <entt/entt.hpp>

namespace
{
	template<typename T>
	auto do_update_frame(const sf::Time delta, T& frames, components::Renderable& renderable) noexcept -> void
	{
		if (not frames.playing)
		{
			return;
		}

		frames.elapsed_time += delta;
		if (frames.elapsed_time >= frames.frame_duration)
		{
			frames.elapsed_time -= frames.frame_duration;
			frames.current_frame_index += 1;

			if (frames.current_frame_index >= frames.total_frame_count)
			{
				if (frames.looping)
				{
					frames.current_frame_index = 0;
				}
				else
				{
					frames.current_frame_index = frames.total_frame_count - 1;
					frames.playing = false;
				}
			}
		}

		// 更新精灵的纹理矩形
		if constexpr (auto& sprite = renderable.sprite;
			std::is_same_v<T, components::sprite_frame::Uniform>)
		{
			const sf::IntRect current_frame
			{
					// todo: 假定图集只有一行?
					frames.frame_position + sf::Vector2i{static_cast<int>(frames.frame_size.x * frames.current_frame_index), 0},
					frames.frame_size,
			};

			sprite.setTextureRect(current_frame);
		}
		else if constexpr (std::is_same_v<T, components::sprite_frame::Variable>)
		{
			const auto& current_frame = frames.frames[frames.current_frame_index];

			sprite.setTextureRect(current_frame);
			// todo: 原点一定是中心点吗?
			sprite.setOrigin(sf::Vector2f{current_frame.size / 2});
		}
		else
		{
			std::unreachable();
		}
	}
}

namespace systems
{
	auto SpriteFrame::update(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		using namespace components;

		for (const auto frame_view = registry.view<sprite_frame::Uniform, Renderable>();
		     const auto [entity, frames, renderable]: frame_view.each())
		{
			do_update_frame(delta, frames, renderable);
		}

		for (const auto frame_view = registry.view<sprite_frame::Variable, Renderable>();
		     const auto [entity, frames, renderable]: frame_view.each())
		{
			do_update_frame(delta, frames, renderable);
		}
	}
}
