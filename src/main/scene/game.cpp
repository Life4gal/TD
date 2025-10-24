#include <scene/game.hpp>

// ================
// INITIALIZE

#include <initialize/map_data.hpp>
#include <initialize/wave_data.hpp>
#include <initialize/asset.hpp>
#include <initialize/event_connection.hpp>
#include <initialize/game.hpp>
#include <initialize/map.hpp>
#include <initialize/navigation.hpp>
#include <initialize/observer.hpp>
#include <initialize/player.hpp>
#include <initialize/hud.hpp>

// ================
// UPDATE

#include <update/game.hpp>

#include <update/wave.hpp>
#include <update/navigation.hpp>
#include <update/observer.hpp>
#include <update/weapon.hpp>
#include <update/limited_life.hpp>
#include <update/sprite_frame.hpp>

#include <update/player.hpp>
#include <update/graveyard.hpp>
#include <update/resource.hpp>
#include <update/hud.hpp>

// ================
// RENDER

#include <render/map.hpp>
#include <render/navigation.hpp>
#include <render/renderable.hpp>
#include <render/health_bar.hpp>
#include <render/player.hpp>
#include <render/hud.hpp>

// ================
// HELPER

#include <helper/player.hpp>

// ================
// UTILITY

#include <utility/functional.hpp>

// ================
// DEPENDENCIES

#include <entt/entt.hpp>
#include <SFML/Window.hpp>
#include <imgui.h>

namespace scene
{
	auto Game::do_update_simulation(const sf::Time delta) noexcept -> void
	{
		// 更新游戏状态
		update::game_simulation(scene_registry_, delta);

		// 更新波次
		update::wave(scene_registry_, delta);
		// 更新导航
		update::navigation(scene_registry_, delta);
		// 更新观察者
		update::observer(scene_registry_, delta);

		// 更新塔(武器)目标
		update::weapon(scene_registry_, delta);

		// 更新有限生命周期实体
		update::limited_life(scene_registry_, delta);

		// 更新精灵帧序列
		update::sprite_frame(scene_registry_, delta);
	}

	auto Game::do_update(const sf::Time delta) noexcept -> void
	{
		// 更新游戏状态
		update::game(scene_registry_, delta);

		// 更新玩家(检测到达终点敌人)
		update::player(scene_registry_);
		// 更新墓地(击杀敌人产生资源)
		update::graveyard(scene_registry_);
		// 更新资源(获取产生的资源)
		update::resource(scene_registry_);
		// 更新玩家HUD
		update::hud(scene_registry_);
	}

	// todo: 更新全局统计数据等信息?
	Game::~Game() noexcept = default;

	Game::Game(std::shared_ptr<entt::registry> global_registry) noexcept
		: Scene{std::move(global_registry)},
		  simulation_times_per_tick_{1}
	{
		// 载入地图数据
		initialize::map_data(scene_registry_);
		// 载入波次数据
		initialize::wave_data(scene_registry_);

		// 预加载资源
		initialize::asset(scene_registry_);

		// 连接事件
		initialize::event_connection(scene_registry_);

		// 初始化游戏
		initialize::game(scene_registry_);
		// 初始化地图
		initialize::map(scene_registry_);
		// 初始化导航
		initialize::navigation(scene_registry_);
		// 初始化观察者
		initialize::observer(scene_registry_);
		// 初始化玩家
		initialize::player(scene_registry_);
		// 初始化HUD
		initialize::hud(scene_registry_);
	}

	auto Game::handle_event(const sf::Event& event) noexcept -> void
	{
		const auto& io = ImGui::GetIO();
		const auto want_capture_keyboard = io.WantCaptureKeyboard;
		const auto want_capture_mouse = io.WantCaptureMouse;

		if (want_capture_keyboard or want_capture_mouse)
		{
			return;
		}

		event.visit(
			utility::overloads
			{
					[&](const sf::Event::MouseButtonPressed& mbp) noexcept -> void
					{
						if (want_capture_mouse)
						{
							return;
						}

						const auto position = sf::Vector2f{mbp.position};

						if (mbp.button == sf::Mouse::Button::Left)
						{
							helper::Player::try_build_tower(scene_registry_, position);
						}
						else if (mbp.button == sf::Mouse::Button::Right)
						{
							helper::Player::try_destroy_tower(scene_registry_, position);
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

		do_update(delta);
	}

	auto Game::render(sf::RenderWindow& window) noexcept -> void
	{
		// 先渲染地图
		render::map(scene_registry_, window);
		// 绘制导航信息(路径/流向)
		render::navigation(scene_registry_, window);
		// 绘制实体
		render::renderable(scene_registry_, window);
		// 渲染血条
		render::health_bar(scene_registry_, window);
		// 绘制玩家(鼠标)
		render::player(scene_registry_, window);
		// HUD位于最上层
		render::hud(scene_registry_, window);
	}
}
