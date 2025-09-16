#pragma once

#include <variant>
#include <vector>

#include <components/entity.hpp>

#include <SFML/System/Time.hpp>

namespace config::wave
{
	// 索引类型
	using index_type = std::uint32_t;

	// 单个敌人生成配置
	class Spawn
	{
	public:
		using type_type = components::entity::Type;

		// 敌人类型
		type_type type;
		// 出生点
		std::uint32_t gate_id;
		// 该波次开始后多久进行该次生成
		sf::Time delay;
	};

	// 波次结束条件
	class EndCondition
	{
	public:
		// 所有敌人(包含所有波次)全部死亡
		// 一般用于最后一个波次判断,放置最后一个波次敌人被消灭
		// 但是前面波次的敌人还存在,却提前满足所有波次条件的情况
		// 当然也可以用于那些希望场上没有敌人后才刷新的波次
		class Extinction {};

		// 所有敌人(只包含当前波次)全部死亡
		// 一般来说(除最后一个波次外)所有波次都隐含该条件
		// 只要当前波次所有敌人全部死亡,都视为该波次结束
		class Clear {};

		// 到达指定持续时间则视为波次结束(即使当前波次还存在敌人)
		// 如果该波次所有敌人都死亡,即使持续时间还没到也视为结束
		class Duration
		{
		public:
			sf::Time duration;
		};

		// 指定类型敌人全部死亡则视为波次结束(即使当前波次还存在敌人)
		class Type
		{
		public:
			using type_type = components::entity::Type;

			type_type type;
		};

		using end_condition_type = std::variant<Extinction, Clear, Duration, Type>;

		end_condition_type end_condition;
	};

	// 单个波次配置
	class Wave
	{
	public:
		using spawns_type = std::vector<Spawn>;

		// 生成信息
		spawns_type spawns;
		// 结束条件
		EndCondition end_condition;
		// 准备时间,如果为0则不等待
		sf::Time preparation_time;
	};

	// 所有波次配置
	class Waves
	{
	public:
		std::vector<Wave> waves;
	};
}
