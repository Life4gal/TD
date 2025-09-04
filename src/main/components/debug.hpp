#pragma once

#include <SFML/Graphics/VertexArray.hpp>

namespace components
{
	// 显示地面缓存路径
	class DebugRenderPath
	{
	public:
		sf::VertexArray paths;
	};

	// 显示地面洋流方向
	class DebugRenderFlow
	{
	public:
		sf::VertexArray directions;
	};

	// 绘制塔与目标连线
	class DebugRenderTarget
	{
	public:
		sf::VertexArray lines;
	};
}
