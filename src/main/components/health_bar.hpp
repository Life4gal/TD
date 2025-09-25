#pragma once

#include <SFML/Graphics/VertexArray.hpp>

namespace components::health_bar
{
	// enemy::Health是当前血量
	// 这里是满血血量
	class Health
	{
	public:
		float health;
	};

	// 血条大小
	class Size
	{
	public:
		sf::Vector2f size;
	};

	// 血条相比于实体中心偏移(一般取决于动画大小)
	class Offset
	{
	public:
		sf::Vector2f offset;
	};

	// 血条位置
	// entity::position + Offset

	// 用于绘制所有血条的缓冲区
	// todo: 只有一种血条吗?
	class Renderer
	{
	public:
		sf::VertexArray triangles;
	};
}
