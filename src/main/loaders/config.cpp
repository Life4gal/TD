#include <loaders/config.hpp>

#include <fstream>
#include <ranges>

#include <nlohmann/json.hpp>

namespace
{
	using namespace loaders;

	[[nodiscard]] auto load_config_file(const Config::Category category) noexcept -> std::optional<nlohmann::json>
	{
		auto file = [category]
		{
			using enum Config::Category;
			switch (category)
			{
				case SYSTEM:
				{
					return std::ifstream{"system-config.json"};
				}
				case WINDOW:
				{
					return std::ifstream{"window-config.json"};
				}
				default: // NOLINT(clang-diagnostic-covered-switch-default)
				{
					std::unreachable();
				}
			}
		}();

		if (not file.is_open())
		{
			return std::nullopt;
		}

		auto config = nlohmann::json::parse(file, nullptr, false);

		if (config.is_discarded())
		{
			return std::nullopt;
		}

		return config;
	}

	template<typename T>
	[[nodiscard]] auto get_config(const nlohmann::json& json, const std::span<T> names) noexcept -> std::optional<nlohmann::json>
	{
		assert(not names.empty());

		auto iterator = json.find(names.front());
		if (iterator == json.end())
		{
			return std::nullopt;
		}

		for (const auto& name: names | std::views::drop(1))
		{
			if (iterator = iterator.value().find(name);
				iterator == json.end())
			{
				return std::nullopt;
			}
		}

		return iterator.value();
	}

	template<typename T>
	[[nodiscard]] auto load_and_get_config(const Config::Category category, const std::span<T> names) noexcept -> std::optional<nlohmann::json>
	{
		assert(not names.empty());

		const auto config_file = load_config_file(category);
		if (not config_file.has_value())
		{
			return std::nullopt;
		}

		return ::get_config<T>(*config_file, names);
	}
}

namespace loaders
{
	auto Config::operator()(const Category category, const std::string_view name) noexcept -> result_type
	{
		if (auto config = load_and_get_config(category, std::span{&name, 1});
			config.has_value())
		{
			return std::make_shared<nlohmann::json>(*std::move(config));
		}

		return nullptr;
	}

	auto Config::operator()(const Category category, const std::span<const char*> names) noexcept -> result_type
	{
		assert(not names.empty());

		if (auto config = load_and_get_config(category, names);
			config.has_value())
		{
			return std::make_shared<nlohmann::json>(*std::move(config));
		}

		return nullptr;
	}

	auto Config::operator()(const Category category, const std::span<const std::string_view> names) noexcept -> result_type
	{
		assert(not names.empty());

		if (auto config = load_and_get_config(category, names);
			config.has_value())
		{
			return std::make_shared<nlohmann::json>(*std::move(config));
		}

		return nullptr;
	}

	auto Config::operator()(const Category category, const std::span<const std::string> names) noexcept -> result_type
	{
		assert(not names.empty());

		if (auto config = load_and_get_config(category, names);
			config.has_value())
		{
			return std::make_shared<nlohmann::json>(*std::move(config));
		}

		return nullptr;
	}
}
