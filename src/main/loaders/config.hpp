#pragma once

#include <memory>
#include <string>
#include <span>

#include <nlohmann/json_fwd.hpp>

namespace loaders
{
	class Config
	{
	public:
		using config_type = nlohmann::json;
		using result_type = std::shared_ptr<config_type>;

		enum class Category : std::uint8_t
		{
			SYSTEM,
			WINDOW,
			//
		};

		[[nodiscard]] static auto operator()(Category category, std::string_view name) noexcept -> result_type;

		[[nodiscard]] static auto operator()(Category category, std::span<const char*> names) noexcept -> result_type;

		[[nodiscard]] static auto operator()(Category category, std::span<const std::string_view> names) noexcept -> result_type;

		[[nodiscard]] static auto operator()(Category category, std::span<const std::string> names) noexcept -> result_type;
	};
}
