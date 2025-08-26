#include <map.hpp>

#include <external/imgui-SFML.hpp>

#include <SFML/Graphics.hpp>

#include <imgui.h>

auto main() noexcept -> int
{
	constexpr int window_width = 1920;
	constexpr int window_height = 1080;

	const sf::VideoMode video_mode{{window_width, window_height}};
	sf::RenderWindow window{video_mode, "MapEditor"};
	window.setFramerateLimit(60);

	sf::Clock delta_clock;

	{
		IMGUI_CHECKVERSION();

		// const auto init = ImGui::SFML::Init(window);
		// assert(init);

		const auto init = ImGui::SFML::Init(window, false);
		assert(init);

		auto* fonts = ImGui::GetIO().Fonts;
		fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\msyh.ttc)", 16.f, nullptr, fonts->GetGlyphRangesChineseSimplifiedCommon());

		const auto update = ImGui::SFML::UpdateFontTexture();
		assert(update);
	}

	auto& imgui_io = ImGui::GetIO();
	imgui_io.IniFilename = "map_editor.ini";
	imgui_io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	editor::Map map{};

	map.setup();

	// 主循环
	while (window.isOpen())
	{
		while (const auto event = window.pollEvent())
		{
			ImGui::SFML::ProcessEvent(window, *event);

			if (event->is<sf::Event::Closed>())
			{
				window.close();
			}
		}

		const auto delta = delta_clock.restart();

		ImGui::SFML::Update(window, delta);

		map.update();

		window.clear({40, 44, 52});

		ImGui::SFML::Render(window);

		window.display();
	}

	ImGui::SFML::Shutdown();
	return 0;
}
