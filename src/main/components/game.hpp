#pragma once

#include <SFML/System/Time.hpp>

namespace components::game
{
	// 上一帧历时
	class FrameDelta
	{
	public:
		sf::Time delta;
	};

	// 游戏总历时(Game::do_update)
	// 基于现实时间
	class ElapsedTime
	{
	public:
		sf::Time elapsed;
	};

	// 游戏模拟历时(Game::do_update_simulation)
	// 基于模拟时间(一般与总历时相同,如果加速则会大于总历时)
	class ElapsedSimulationTime
	{
	public:
		sf::Time elapsed;
	};
}
