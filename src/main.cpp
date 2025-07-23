#include <print>

// =========================
// SYSTEMS

#include <systems/map.hpp>
#include <systems/hud.hpp>
#include <systems/navigation.hpp>
#include <systems/player.hpp>
#include <systems/tower.hpp>
#include <systems/enemy.hpp>
#include <systems/graveyard.hpp>

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

		auto initialize() noexcept -> void
		{
			systems::Map::initialize(registry);
			systems::Navigation::initialize(registry);
			systems::Player::initialize(registry);
			systems::HUD::initialize(registry);

			systems::Tower::initialize(registry);

			systems::Enemy::initialize(registry);
			systems::Graveyard::initialize(registry);
		}

		auto update_simulation(const sf::Time delta) noexcept -> void
		{
			systems::Navigation::update(registry, delta);

			systems::Tower::update(registry, delta);

			systems::Enemy::update(registry, delta);
		}

		auto update() noexcept -> void
		{
			systems::Map::update(registry);
			systems::Player::update(registry);
			systems::HUD::update(registry);

			systems::Graveyard::update(registry);
		}

		auto render(sf::RenderWindow& window) noexcept -> void
		{
			systems::Map::render(registry, window);
			systems::Navigation::render(registry, window);
			systems::Player::render(registry, window);
			systems::HUD::render(registry, window);

			systems::Tower::render(registry, window);

			systems::Enemy::render(registry, window);
			systems::Graveyard::render(registry, window);
		}

		auto build_tower(const sf::Vector2f world_position) noexcept -> void
		{
			systems::Player::try_build_tower(registry, world_position);
		}

		auto destroy_tower(const sf::Vector2f world_position) noexcept -> void
		{
			systems::Player::try_destroy_tower(registry, world_position);
		}
	};
}

auto main() noexcept -> int
{
	constexpr int window_width = 1920;
	constexpr int window_height = 1080;

	Game game{};
	game.initialize();

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

							if (mbp.button == sf::Mouse::Button::Left)
							{
								game.build_tower(position);
							}
							else if (mbp.button == sf::Mouse::Button::Right)
							{
								game.destroy_tower(position);
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
