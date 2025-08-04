#include <system/game/initialize/hud.hpp>

#include <component/game/render.hpp>

#include <entt/entt.hpp>

namespace game::system::initialize
{
	auto hud(entt::registry& registry) noexcept -> void
	{
		using namespace component;

		auto& font_manager = manager::Font::instance();
		const auto hud_font_id = font_manager.precache(R"(C:\Windows\Fonts\msyh.ttc)");
		assert(hud_font_id.has_value());

		render::HUD render_hud
		{
				.hud_font = *hud_font_id,
				.hud_text = {font_manager.font_of(*hud_font_id), "", 25},
		};

		registry.ctx().emplace<render::HUD>(std::move(render_hud));
	}
}
