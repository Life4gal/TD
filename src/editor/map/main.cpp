#include <map.hpp>

#include <external/imgui-SFML.hpp>

#include <SFML/Graphics.hpp>

#include <imgui.h>

auto main() noexcept -> int
{
	sf::RenderWindow window{sf::VideoMode{{1920, 1080}}, "MapEditor"};
	window.setFramerateLimit(60);

	sf::Clock delta_clock;

	if (not ImGui::SFML::Init(window))
	{
		return 1;
	}

	ImGui::GetIO().IniFilename = "map_editor.ini";

	editor::Map map{};

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

			const auto& io = ImGui::GetIO();
			const auto want_mouse = io.WantCaptureMouse;
			const auto want_keyboard = io.WantCaptureKeyboard;

			if (not want_mouse)
			{
				// 鼠标点击放置瓦片
				if (const auto mbp = event->getIf<sf::Event::MouseButtonPressed>();
					mbp && mbp->button == sf::Mouse::Button::Left)
				{
					map.place_tile_map(sf::Vector2u{mbp->position});
				}

				// 鼠标拖放放置瓦片
				if (const auto mv = event->getIf<sf::Event::MouseMoved>();
					mv && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
				{
					map.place_tile_map(sf::Vector2u{mv->position});
				}
			}
		}

		ImGui::SFML::Update(window, delta_clock.restart());

		ImGui::Begin("MapEditor");
		map.show();
		ImGui::End();

		window.clear({40, 44, 52});

		map.render(window);

		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();
	return 0;
}
