#pragma once

#include <utility>

namespace components
{
	enum class ResourceType : std::uint8_t
	{
		// 生命值
		HEALTH,
		// 魔法值(用于释放技能)
		MANA,
		// 金币
		GOLD,
		//
	};

	class Resource
	{
	public:
		using size_type = std::uint64_t;

	private:
		size_type type_ : 8;
		size_type count_ : 64 - 8;

	public:
		Resource(const ResourceType type, const size_type count) noexcept
			: type_{std::to_underlying(type)},
			  count_{count} {}

		[[nodiscard]] constexpr auto type() const noexcept -> ResourceType
		{
			return static_cast<ResourceType>(type_);
		}

		[[nodiscard]] constexpr auto count() const noexcept -> size_type
		{
			return count_;
		}
	};
}
