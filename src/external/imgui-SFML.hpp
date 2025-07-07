#ifndef IMGUI_SFML_H
#define IMGUI_SFML_H

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Joystick.hpp>

#include <optional>

namespace sf
{
	class Event;
	class RenderTarget;
	class RenderTexture;
	class RenderWindow;
	class Sprite;
	class Texture;
	class Window;
} // namespace sf

namespace ImGui
{
	namespace SFML
	{
		[[nodiscard]] bool Init(sf::RenderWindow& window, bool loadDefaultFont = true);
		[[nodiscard]] bool Init(sf::Window& window, sf::RenderTarget& target, bool loadDefaultFont = true);
		[[nodiscard]] bool Init(sf::Window& window, const sf::Vector2f& displaySize, bool loadDefaultFont = true);

		void SetCurrentWindow(const sf::Window& window);
		void ProcessEvent(const sf::Window& window, const sf::Event& event);

		void Update(sf::RenderWindow& window, sf::Time dt);
		void Update(sf::Window& window, sf::RenderTarget& target, sf::Time dt);
		void Update(const sf::Vector2i& mousePos, const sf::Vector2f& displaySize, sf::Time dt);

		void Render(sf::RenderWindow& window);
		void Render(sf::RenderTarget& target);
		void Render();

		void Shutdown(const sf::Window& window);
		// Shuts down all ImGui contexts
		void Shutdown();

		[[nodiscard]] bool UpdateFontTexture();
		std::optional<sf::Texture>& GetFontTexture();

		// joystick functions
		void SetActiveJoystickId(unsigned int joystickId);
		void SetJoystickDPadThreshold(float threshold);
		void SetJoystickLStickThreshold(float threshold);
		void SetJoystickRStickThreshold(float threshold);
		void SetJoystickLTriggerThreshold(float threshold);
		void SetJoystickRTriggerThreshold(float threshold);

		void SetJoystickMapping(int key, unsigned int joystickButton);
		void SetDPadXAxis(sf::Joystick::Axis dPadXAxis, bool inverted = false);
		void SetDPadYAxis(sf::Joystick::Axis dPadYAxis, bool inverted = false);
		void SetLStickXAxis(sf::Joystick::Axis lStickXAxis, bool inverted = false);
		void SetLStickYAxis(sf::Joystick::Axis lStickYAxis, bool inverted = false);
		void SetRStickXAxis(sf::Joystick::Axis rStickXAxis, bool inverted = false);
		void SetRStickYAxis(sf::Joystick::Axis rStickYAxis, bool inverted = false);
		void SetLTriggerAxis(sf::Joystick::Axis lTriggerAxis);
		void SetRTriggerAxis(sf::Joystick::Axis rTriggerAxis);
	} // end of namespace SFML

	// custom SFML overloads for ImGui widgets

	// Image overloads for sf::Texture
	void Image(
		const sf::Texture& texture,
		const sf::Color& tintColor = sf::Color::White,
		const sf::Color& borderColor = sf::Color::Transparent
	);
	void Image(
		const sf::Texture& texture,
		const sf::Vector2f& size,
		const sf::Color& tintColor = sf::Color::White,
		const sf::Color& borderColor = sf::Color::Transparent
	);

	// Image overloads for sf::RenderTexture
	void Image(
		const sf::RenderTexture& texture,
		const sf::Color& tintColor = sf::Color::White,
		const sf::Color& borderColor = sf::Color::Transparent
	);
	void Image(
		const sf::RenderTexture& texture,
		const sf::Vector2f& size,
		const sf::Color& tintColor = sf::Color::White,
		const sf::Color& borderColor = sf::Color::Transparent
	);

	// Image overloads for sf::Sprite
	void Image(
		const sf::Sprite& sprite,
		const sf::Color& tintColor = sf::Color::White,
		const sf::Color& borderColor = sf::Color::Transparent
	);
	void Image(
		const sf::Sprite& sprite,
		const sf::Vector2f& size,
		const sf::Color& tintColor = sf::Color::White,
		const sf::Color& borderColor = sf::Color::Transparent
	);

	// ImageButton overloads for sf::Texture
	bool ImageButton(
		const char* id,
		const sf::Texture& texture,
		const sf::Vector2f& size,
		const sf::Color& bgColor = sf::Color::Transparent,
		const sf::Color& tintColor = sf::Color::White
	);

	// ImageButton overloads for sf::RenderTexture
	bool ImageButton(
		const char* id,
		const sf::RenderTexture& texture,
		const sf::Vector2f& size,
		const sf::Color& bgColor = sf::Color::Transparent,
		const sf::Color& tintColor = sf::Color::White
	);

	// ImageButton overloads for sf::Sprite
	bool ImageButton(
		const char* id,
		const sf::Sprite& sprite,
		const sf::Vector2f& size,
		const sf::Color& bgColor = sf::Color::Transparent,
		const sf::Color& tintColor = sf::Color::White
	);

	// Draw_list overloads. All positions are in relative coordinates (relative to top-left of the
	// current window)
	void DrawLine(const sf::Vector2f& a, const sf::Vector2f& b, const sf::Color& col, float thickness = 1.0f);
	void DrawRect(
		const sf::FloatRect& rect,
		const sf::Color& color,
		float rounding = 0.0f,
		int rounding_corners = 0x0F,
		float thickness = 1.0f
	);
	void DrawRectFilled(
		const sf::FloatRect& rect,
		const sf::Color& color,
		float rounding = 0.0f,
		int rounding_corners = 0x0F
	);
} // end of namespace ImGui

#endif // # IMGUI_SFML_H
