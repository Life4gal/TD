#include <print>

// =====================================
// SCENE
#include <scene/game.hpp>

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

auto main() noexcept -> int
{
	constexpr int window_width = 1920;
	constexpr int window_height = 1080;

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

	auto global_registry = std::make_shared<entt::registry>();

	// 载入场景
	const auto current_scene = std::make_unique<scene::Game>(global_registry);

	while (window.isOpen())
	{
		while (const auto event = window.pollEvent())
		{
			ImGui::SFML::ProcessEvent(window, *event);

			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}
			else
			{
				current_scene->handle_event(*event);
			}
		}

		const auto delta = delta_clock.restart();

		ImGui::SFML::Update(window, delta);

		current_scene->update(delta);

		// ImGui::ShowDemoWindow();

		window.clear({45, 45, 45});

		current_scene->render(window);
		ImGui::SFML::Render(window);

		window.display();
	}

	ImGui::SFML::Shutdown(window);
}
