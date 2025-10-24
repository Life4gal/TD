#include <initialize/asset.hpp>

#include <components/asset.hpp>

#include <entt/entt.hpp>

namespace initialize
{
	auto asset(entt::registry& registry) noexcept -> void
	{
		using namespace components;

		// 资源管理器
		auto& [configs] = registry.ctx().emplace<asset::Configs>();
		auto& [fonts] = registry.ctx().emplace<asset::Fonts>();
		auto& [textures] = registry.ctx().emplace<asset::Textures>();
		auto& [sounds] = registry.ctx().emplace<asset::Sounds>();

		// 预加载所有需要的资源

		// 字体
		{
			// HUD字体
			{
				auto [it, result] = fonts.load(asset::constants::hud, "hud");
				assert(it->second);
			}

			std::ignore = fonts;
		}

		// 纹理
		{
			// 地图纹理
			{
				auto [it, result] = textures.load(asset::constants::map, loaders::TextureType::MAP, "map1");
				assert(it->second);
			}

			// 敌人纹理
			{
				constexpr std::array<std::string_view, 3> names
				{
						"deep-dive-AntleredRascal",
						"deep-dive-WarpSkull",
						"deep-dive-BloodshotEye"
				};

				std::ranges::for_each(
					names,
					[&](const auto name) noexcept -> void
					{
						const entt::basic_hashed_string hash_name{name.data(), name.size()};

						auto [it, result] = textures.load(hash_name, loaders::TextureType::ENEMY, name);
						assert(it->second);
					}
				);
			}
		}

		// 声音
		{
			// 攻击音效
			{
				constexpr std::array<std::string_view, 1> names
				{
						"laser"
				};

				std::ranges::for_each(
					names,
					[&](const auto name) noexcept -> void
					{
						const entt::basic_hashed_string hash_name{name.data(), name.size()};

						auto [it, result] = sounds.load(hash_name, name);
						assert(it->second);
					}
				);
			}
		}
	}
}
