#include <update/sprite_frame.hpp>

#include <components/sprite_frame.hpp>
#include <components/renderable.hpp>

#include <helper/sprite_frame.hpp>

#include <entt/entt.hpp>

namespace update
{
	auto sprite_frame(entt::registry& registry, const sf::Time delta) noexcept -> void
	{
		using namespace components;

		for (const auto sprite_frame_view = registry.view<sprite_frame::Timer, sprite_frame::Frame, sprite_frame::Condition, Renderable>();
		     const auto [entity, timer, frame, condition, renderable]: sprite_frame_view.each())
		{
			if (not condition.playing)
			{
				return;
			}

			timer.elapsed_time += delta;
			if (timer.elapsed_time >= timer.frame_duration)
			{
				timer.elapsed_time -= timer.frame_duration;
				frame.current_index += 1;

				if (frame.current_index >= frame.total_count)
				{
					if (condition.looping)
					{
						frame.current_index = 0;
					}
					else
					{
						frame.current_index = frame.total_count - 1;
						condition.playing = false;
					}
				}

				// 更新精灵的纹理矩形
				const auto rect = helper::SpriteFrame::rect_of(registry, entity);
				renderable.sprite.setTextureRect(rect);
				// todo: 原点一定是中心点吗?
				renderable.sprite.setOrigin(sf::Vector2f{rect.size / 2});
			}
		}
	}
}
