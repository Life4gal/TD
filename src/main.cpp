#include <print>

// =========================
// SYSTEMS

#include <systems/map.hpp>
#include <systems/navigation.hpp>

#include <systems/enemy.hpp>

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

			systems::Enemy::initialize(registry);
		}

		auto update(const sf::Time delta) noexcept -> void
		{
			systems::Map::update(registry, delta);
			systems::Navigation::update(registry, delta);

			systems::Enemy::update(registry, delta);
		}

		auto render(sf::RenderWindow& window) noexcept -> void
		{
			systems::Map::render(registry, window);
			systems::Navigation::render(registry, window);

			systems::Enemy::render(registry, window);
		}
	};
}

auto main() noexcept -> int
{
	constexpr int window_width = 1920;
	constexpr int window_height = 1080;

	Game game{};
	game.initialize();

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
						//
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
		game.update(delta);

		ImGui::ShowDemoWindow();

		ImGui::Begin("测试窗口");
		ImGui::Text("hello world!");
		ImGui::End();

		window.clear({45, 45, 45});

		game.render(window);
		ImGui::SFML::Render(window);

		window.display();
	}

	ImGui::SFML::Shutdown(window);
}
