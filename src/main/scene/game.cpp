#include <scene/game.hpp>

#include <components/config.hpp>
#include <components/font.hpp>
#include <components/sound.hpp>
#include <components/texture.hpp>

#include <systems/initialize.hpp>

#include <systems/map.hpp>
#include <systems/wave.hpp>
#include <systems/navigation.hpp>
#include <systems/observer.hpp>

#include <systems/resource.hpp>
#include <systems/player.hpp>
#include <systems/graveyard.hpp>
#include <systems/hud.hpp>
#include <systems/weapon.hpp>
#include <systems/timed_life.hpp>
#include <systems/sprite_frame.hpp>
#include <systems/renderable.hpp>
#include <systems/health_bar.hpp>

#include <systems/debug.hpp>

#include <helper/map.hpp>
#include <helper/player.hpp>
#include <helper/wave.hpp>

#include <utility/functional.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

#include <imgui.h>

namespace scene
{
	auto Game::do_update_simulation(const sf::Time delta) noexcept -> void
	{
		// 更新波次
		systems::Wave::update(scene_registry_, delta);
		// 更新导航
		systems::Navigation::update(scene_registry_, delta);
		// 更新观察者
		systems::Observer::update(scene_registry_, delta);

		// 更新塔(武器)目标
		systems::Weapon::update(scene_registry_, delta);

		// 更新定长生命周期实体
		systems::TimedLife::update(scene_registry_, delta);

		// 更新精灵帧序列
		systems::SpriteFrame::update(scene_registry_, delta);
	}

	auto Game::do_update() noexcept -> void
	{
		// 更新玩家(检测到达终点敌人)
		systems::Player::update(scene_registry_);
		// 更新墓地(击杀敌人产生资源)
		systems::Graveyard::update(scene_registry_);
		// 更新资源(获取产生的资源)
		systems::Resource::update(scene_registry_);
		// 更新玩家HUD
		systems::HUD::update(scene_registry_);
	}

	// todo: 更新全局统计数据等信息?
	Game::~Game() noexcept = default;

	Game::Game(std::shared_ptr<entt::registry> global_registry) noexcept
		: Scene{std::move(global_registry)},
		  simulation_times_per_tick_{1}
	{
		// 资源管理器
		scene_registry_.ctx().emplace<components::Configs>();
		scene_registry_.ctx().emplace<components::Fonts>();
		scene_registry_.ctx().emplace<components::Textures>();
		scene_registry_.ctx().emplace<components::Sounds>();

		// 载入地图
		helper::Map::load(scene_registry_);
		// 载入波次
		helper::Wave::load(scene_registry_);

		// 初始化游戏
		systems::Initialize::initialize(scene_registry_);
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

		do_update();
	}

	auto Game::render(sf::RenderWindow& window) noexcept -> void
	{
		// 先渲染地图
		systems::Map::render(scene_registry_, window);

		// 绘制实体
		systems::Renderable::render(scene_registry_, window);

		// 渲染血条
		systems::HealthBar::render(scene_registry_, window);

		systems::Debug::render(scene_registry_, window);

		// HUD位于最上层
		systems::HUD::render(scene_registry_, window);
	}
}
