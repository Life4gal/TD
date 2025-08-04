#include <scene/game.hpp>

#include <system/game/initialize/player.hpp>
#include <system/game/initialize/map.hpp>
#include <system/game/initialize/navigation.hpp>
#include <system/game/initialize/resource.hpp>
#include <system/game/initialize/hud.hpp>
#include <system/game/initialize/enemy.hpp>
#include <system/game/initialize/tower.hpp>
#include <system/game/initialize/weapon.hpp>

#include <system/game/update/player.hpp>
#include <system/game/update/map.hpp>
#include <system/game/update/navigation.hpp>
#include <system/game/update/graveyard.hpp>
#include <system/game/update/resource.hpp>
#include <system/game/update/hud.hpp>
#include <system/game/update/enemy.hpp>
#include <system/game/update/tower.hpp>
#include <system/game/update/weapon.hpp>

#include <system/game/render/player.hpp>
#include <system/game/render/map.hpp>
#include <system/game/render/navigation.hpp>
#include <system/game/render/hud.hpp>
#include <system/game/render/enemy.hpp>
#include <system/game/render/tower.hpp>
#include <system/game/render/weapon.hpp>

#include <system/game/helper/map.hpp>
#include <system/game/helper/player.hpp>

#include <component/game/map.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

#include <imgui.h>

namespace
{
	template<typename... Ts>
	struct overloads : Ts...
	{
		using Ts::operator()...;
	};
}

namespace scene
{
	auto Game::do_update_simulation(const sf::Time delta) noexcept -> void
	{
		using namespace game::system;

		// 更新导航
		update::navigation(scene_registry_, delta);
		// 更新塔
		update::tower(scene_registry_, delta);
		// 更新武器
		update::weapon(scene_registry_, delta);
		// 更新敌人
		update::enemy(scene_registry_, delta);
	}

	auto Game::do_update() noexcept -> void
	{
		using namespace game::system;

		// 更新地图
		update::map(scene_registry_);
		// 更新墓地
		update::graveyard(scene_registry_);
		// 更新资源
		update::resource(scene_registry_);
		// 更新玩家
		update::player(scene_registry_);
		// 更新玩家HUD
		update::hud(scene_registry_);
	}

	// todo: 更新全局统计数据等信息?
	Game::~Game() noexcept = default;

	Game::Game(std::shared_ptr<entt::registry> global_registry) noexcept
		: Scene{std::move(global_registry)},
		  simulation_times_per_tick_{1}
	{
		using namespace game::system;

		// todo: 设定要载入的地图
		using namespace game::component;
		scene_registry_.ctx().emplace<map_ex::Info>("???");

		// 载入地图
		helper::Map::load(scene_registry_);

		// 初始化地图
		initialize::map(scene_registry_);
		// 初始化导航
		initialize::navigation(scene_registry_);
		// 初始化玩家
		initialize::player(scene_registry_);
		// 初始化玩家资源
		initialize::resource(scene_registry_);
		// 初始化玩家HUD
		initialize::hud(scene_registry_);
		// 初始化敌人
		initialize::enemy(scene_registry_);
		// 初始化塔
		initialize::tower(scene_registry_);
		// 初始化武器
		initialize::weapon(scene_registry_);
	}

	auto Game::handle_event(const sf::Event& event) noexcept -> void
	{
		using namespace game::system;

		const auto& io = ImGui::GetIO();
		const auto want_capture_keyboard = io.WantCaptureKeyboard;
		const auto want_capture_mouse = io.WantCaptureMouse;

		if (want_capture_keyboard or want_capture_mouse)
		{
			return;
		}

		event.visit(
			overloads
			{
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
								helper::Player::try_equip_weapon(scene_registry_, position);
							}
							else
							{
								helper::Player::try_build_tower(scene_registry_, position);
							}
						}
						else if (mbp.button == sf::Mouse::Button::Right)
						{
							if (pressed_ctrl)
							{
								helper::Player::try_remove_weapon(scene_registry_, position);
							}
							else
							{
								helper::Player::try_destroy_tower(scene_registry_, position);
							}
						}
					},
					[&](const sf::Event::KeyPressed& kp) noexcept -> void
					{
						if (want_capture_keyboard)
						{
							return;
						}

						if (kp.code == sf::Keyboard::Key::Space)
						{
							simulation_times_per_tick_ = simulation_times_per_tick_ == 0 ? 1 : 0;
						}
						else if (kp.code == sf::Keyboard::Key::Tab)
						{
							simulation_times_per_tick_ = 10;
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
							simulation_times_per_tick_ = 1;
						}
					},
					// unhandled
					[]<typename T>(const T& e) noexcept -> void
					{
						std::ignore = e;
					},
			}
		);
	}

	auto Game::update(const sf::Time delta) noexcept -> void
	{
		for (std::uint32_t current_simulation_tick = 0; current_simulation_tick < simulation_times_per_tick_; ++current_simulation_tick)
		{
			do_update_simulation(delta);
		}

		do_update();
	}

	auto Game::render(sf::RenderWindow& window) noexcept -> void
	{
		using namespace game::system;

		// 地图和导航位于最底层
		render::map(scene_registry_, window);
		render::navigation(scene_registry_, window);
		// 绘制塔/武器/敌人
		render::tower(scene_registry_, window);
		render::weapon(scene_registry_, window);
		render::enemy(scene_registry_, window);
		// 玩家位于最上层
		render::player(scene_registry_, window);
		render::hud(scene_registry_, window);
	}
}
