#include <print>

// =========================
// SYSTEMS

#include <systems/initialize/player.hpp>
#include <systems/initialize/map.hpp>
#include <systems/initialize/navigation.hpp>
#include <systems/initialize/resource.hpp>
#include <systems/initialize/hud.hpp>
#include <systems/initialize/enemy.hpp>
#include <systems/initialize/tower.hpp>
#include <systems/initialize/weapon.hpp>

#include <systems/update/player.hpp>
#include <systems/update/map.hpp>
#include <systems/update/navigation.hpp>
#include <systems/update/graveyard.hpp>
#include <systems/update/resource.hpp>
#include <systems/update/hud.hpp>
#include <systems/update/enemy.hpp>
#include <systems/update/tower.hpp>
#include <systems/update/weapon.hpp>

#include <systems/render/player.hpp>
#include <systems/render/map.hpp>
#include <systems/render/navigation.hpp>
#include <systems/render/hud.hpp>
#include <systems/render/enemy.hpp>
#include <systems/render/tower.hpp>
#include <systems/render/weapon.hpp>

#include <systems/helper/map.hpp>
#include <systems/helper/player.hpp>

// =========================
// ENTT

#include <entt/entt.hpp>

// =========================
// SFML

#include <SFML/Graphics.hpp>

// =========================
// IMGUI

#include <imgui.h>
#include <external/imgui-SFML.hpp>

namespace
{
	template<typename... Ts>
	struct overloads : Ts...
	{
		using Ts::operator()...;
	};

	class Game
	{
	public:
		entt::registry registry;

		// 创建游戏
		auto create() noexcept -> void
		{
			using namespace systems;

			// todo
		}

		auto load_map() noexcept -> void
		{
			using namespace systems;

			// 载入地图
			helper::Map::load(registry);
			// 初始化地图
			initialize::map(registry);
			// 初始化导航
			initialize::navigation(registry);
			// 初始化玩家
			initialize::player(registry);
			// 初始化玩家资源
			initialize::resource(registry);
			// 初始化玩家HUD
			initialize::hud(registry);
			// 初始化敌人
			initialize::enemy(registry);
			// 初始化塔
			initialize::tower(registry);
			// 初始化武器
			initialize::weapon(registry);
		}

		auto update_simulation(const sf::Time delta) noexcept -> void
		{
			using namespace systems;

			// 更新导航
			update::navigation(registry, delta);
			// 更新塔
			update::tower(registry, delta);
			// 更新武器
			update::weapon(registry, delta);
			// 更新敌人
			update::enemy(registry, delta);
		}

		auto update() noexcept -> void
		{
			using namespace systems;

			// 更新地图
			update::map(registry);
			// 更新墓地
			update::graveyard(registry);
			// 更新资源
			update::resource(registry);
			// 更新玩家
			update::player(registry);
			// 更新玩家HUD
			update::hud(registry);
		}

		auto render(sf::RenderWindow& window) noexcept -> void
		{
			using namespace systems;

			// 地图和导航位于最底层
			render::map(registry, window);
			render::navigation(registry, window);
			// 绘制塔/武器/敌人
			render::tower(registry, window);
			render::weapon(registry, window);
			render::enemy(registry, window);
			// 玩家位于最上层
			render::player(registry, window);
			render::hud(registry, window);
		}

		auto build_tower(const sf::Vector2f world_position) noexcept -> void
		{
			using namespace systems;

			helper::Player::try_build_tower(registry, world_position);
		}

		auto destroy_tower(const sf::Vector2f world_position) noexcept -> void
		{
			using namespace systems;

			helper::Player::try_destroy_tower(registry, world_position);
		}

		auto equip_weapon(const sf::Vector2f world_position) noexcept -> void
		{
			using namespace systems;

			helper::Player::try_equip_weapon(registry, world_position);
		}

		auto remove_weapon(const sf::Vector2f world_position) noexcept -> void
		{
			using namespace systems;

			helper::Player::try_remove_weapon(registry, world_position);
		}
	};
}

auto main() noexcept -> int
{
	constexpr int window_width = 1920;
	constexpr int window_height = 1080;

	Game game{};
	game.create();
	// 载入地图
	game.load_map();

	std::uint32_t simulation_times_per_tick = 1;

	const sf::VideoMode video_mode{{window_width, window_height}};
	const auto window_title = TD_PROJECT_NAME " (" TD_BUILD_TYPE ") " TD_GIT_COMMIT_INFO;
	sf::RenderWindow window{video_mode, window_title};

	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);

	sf::Clock delta_clock{};

	{
		// 默认字体
		// const auto init = ImGui::SFML::Init(window);
		// assert(init);

		// 自定义字体
		const auto init = ImGui::SFML::Init(window, false);
		assert(init);

		auto* fonts = ImGui::GetIO().Fonts;
		fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\msyh.ttc)", 16.f, nullptr, fonts->GetGlyphRangesChineseSimplifiedCommon());

		const auto update = ImGui::SFML::UpdateFontTexture();
		assert(update);
	}

	while (window.isOpen())
	{
		while (const auto event = window.pollEvent())
		{
			ImGui::SFML::ProcessEvent(window, *event);

			const auto& io = ImGui::GetIO();
			const auto want_capture_keyboard = io.WantCaptureKeyboard;
			const auto want_capture_mouse = io.WantCaptureMouse;

			if (want_capture_keyboard and want_capture_mouse)
			{
				continue;
			}

			event->visit(
				overloads{
						[&](const sf::Event::Closed) noexcept -> void
						{
							window.close();
						},
						[&](const sf::Event::MouseButtonPressed& mbp) noexcept -> void
						{
							if (want_capture_mouse)
							{
								return;
							}

							const auto position = sf::Vector2f{mbp.position};
							const auto pressed_ctrl = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl);

							if (mbp.button == sf::Mouse::Button::Left)
							{
								if (pressed_ctrl)
								{
									game.equip_weapon(position);
								}
								else
								{
									game.build_tower(position);
								}
							}
							else if (mbp.button == sf::Mouse::Button::Right)
							{
								if (pressed_ctrl)
								{
									game.remove_weapon(position);
								}
								else
								{
									game.destroy_tower(position);
								}
							}
						},
						[&](const sf::Event::MouseButtonReleased& mbr) noexcept -> void
						{
							if (want_capture_mouse)
							{
								return;
							}

							std::ignore = mbr;
						},
						[&](const sf::Event::KeyPressed& kp) noexcept -> void
						{
							if (want_capture_keyboard)
							{
								return;
							}

							if (kp.code == sf::Keyboard::Key::Space)
							{
								simulation_times_per_tick = simulation_times_per_tick == 0 ? 1 : 0;
							}
							else if (kp.code == sf::Keyboard::Key::Tab)
							{
								simulation_times_per_tick = 10;
							}
						},
						[&](const sf::Event::KeyReleased& kr) noexcept -> void
						{
							if (want_capture_keyboard)
							{
								return;
							}

							if (kr.code == sf::Keyboard::Key::Tab)
							{
								simulation_times_per_tick = 1;
							}
						},
						// unhandled
						[&]<typename T>(const T& e) noexcept -> void
						{
							std::ignore = e;
						}
				}
			);
		}

		const auto delta = delta_clock.restart();

		ImGui::SFML::Update(window, delta);

		for (std::uint32_t current_simulation_tick = 0; current_simulation_tick < simulation_times_per_tick; ++current_simulation_tick)
		{
			game.update_simulation(delta);
		}
		game.update();

		// ImGui::ShowDemoWindow();

		window.clear({45, 45, 45});

		game.render(window);
		ImGui::SFML::Render(window);

		window.display();
	}

	ImGui::SFML::Shutdown(window);
}
