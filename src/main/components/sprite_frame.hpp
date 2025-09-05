#pragma once

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace components::sprite_frame
{
	// 统一大小矩形帧动画
	class Uniform
	{
	public:
		// 每帧持续时间
		sf::Time frame_duration;
		// 当前帧已过去时间
		sf::Time elapsed_time;

		// 纹理矩形位置
		// 一般为(0,line*frame_size.y)
		sf::Vector2i frame_position;
		// 纹理矩形大小(位置=frame_position+frame_size*current_frame_index)
		sf::Vector2i frame_size;
		// 总帧数
		std::size_t total_frame_count;
		// 当前帧索引
		std::size_t current_frame_index;

		// 是否循环播放
		bool looping;
		// 是否正在播放(如果不是循环动画且所有帧都播放完毕则不再播放&更新)
		bool playing;
	};

	// 可变大小矩形帧动画(每一帧的矩形大小可以不同)
	class Variable
	{
	public:
		// 每帧持续时间
		sf::Time frame_duration;
		// 当前帧已过去时间
		sf::Time elapsed_time;

		// 纹理矩形序列
		std::unique_ptr<sf::IntRect[]> frames;
		// 总帧数
		std::size_t total_frame_count;
		// 当前帧索引
		std::size_t current_frame_index;

		// 是否循环播放
		bool looping;
		// 是否正在播放(如果不是循环动画且所有帧都播放完毕则不再播放&更新)
		bool playing;
	};
}
