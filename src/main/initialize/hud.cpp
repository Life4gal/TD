#include <initialize/hud.hpp>

#include <components/game/asset.hpp>
#include <components/game/hud.hpp>

#include <helper/asset.hpp>

#include <entt/entt.hpp>

namespace initialize
{
	auto hud(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		const auto& font = helper::Asset::font_of(registry, asset::constants::hud);
		registry.ctx().emplace<hud::Text>(sf::Text{font, "", 25});
	}
}
