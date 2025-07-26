#include <systems/resource.hpp>

#include <algorithm>
#include <ranges>

#include <components/tags.hpp>
#include <components/resource.hpp>

#include <components/player.hpp>

#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

namespace
{
	// 塔建造需要的资源和拆除返还的资源不同
	enum class Type : std::uint8_t
	{
		ACQUIRE,
		REQUIRE,
	};

	template<Type T>
	[[nodiscard]] constexpr auto entity_type_to_resource(const components::EntityType type) noexcept -> components::Resource
	{
		// todo: 加载配置文件
		std::ignore = type;

		using enum components::ResourceType;
		if constexpr (T == Type::ACQUIRE)
		{
			return {GOLD, 50};
		}
		else if (T == Type::REQUIRE)
		{
			return {GOLD, 100};
		}
		else
		{
			std::unreachable();
		}
	}

	template<Type T>
	[[nodiscard]] constexpr auto entity_type_to_resource(const std::span<const components::EntityType> types) noexcept -> std::vector<components::Resource>
	{
		std::vector<components::Resource> resources{};
		resources.reserve(types.size());

		std::ranges::transform(
			types,
			std::back_inserter(resources),
			static_cast<auto(*)(components::EntityType type) noexcept -> components::Resource>(entity_type_to_resource<T>)
		);

		return resources;
	}
}

namespace systems
{
	auto Resource::initialize(entt::registry& registry) noexcept -> void
	{
		auto& player_data = registry.ctx().get<components::PlayerData>();
		auto& resources = player_data.resources;

		// todo: 加载配置文件
		using enum components::ResourceType;
		resources[HEALTH] = 100;
		resources[MANA] = 100;
		resources[GOLD] = 999999999;
	}

	auto Resource::update(entt::registry& registry) noexcept -> void
	{
		auto& player_data = registry.ctx().get<components::PlayerData>();
		auto& resources = player_data.resources;

		const auto resource_reward_view = registry.view<components::tags::resource, components::Resource>();

		for (const auto [entity, resource]: resource_reward_view.each())
		{
			resources[resource.type()] += resource.count();
		}

		registry.destroy(resource_reward_view.begin(), resource_reward_view.end());
	}

	auto Resource::render(entt::registry& registry, sf::RenderWindow& window) noexcept -> void
	{
		std::ignore = registry;
		std::ignore = window;
	}

	auto Resource::acquire(entt::registry& registry, const std::span<const components::Resource> resources) noexcept -> void
	{
		for (const auto resource: resources)
		{
			acquire(registry, resource);
		}
	}

	auto Resource::acquire(entt::registry& registry, const components::Resource resource) noexcept -> entt::entity
	{
		const auto entity = registry.create();

		registry.emplace<components::Resource>(entity, resource);

		registry.emplace<components::tags::resource>(entity);
		return entity;
	}

	auto Resource::acquire(entt::registry& registry, const std::span<const components::EntityType> types) noexcept -> void
	{
		const auto resources = entity_type_to_resource<Type::ACQUIRE>(types);

		return acquire(registry, resources);
	}

	auto Resource::acquire(entt::registry& registry, const components::EntityType type) noexcept -> entt::entity
	{
		const auto resource = entity_type_to_resource<Type::ACQUIRE>(type);

		return acquire(registry, resource);
	}

	auto Resource::require(entt::registry& registry, const std::span<const components::Resource> resources) noexcept -> bool
	{
		return std::ranges::all_of(
			resources,
			[&](const components::Resource resource) noexcept -> bool
			{
				return require(registry, resource);
			}
		);
	}

	auto Resource::require(entt::registry& registry, const components::Resource resource) noexcept -> bool
	{
		const auto& player_data = registry.ctx().get<components::PlayerData>();
		const auto& player_resources = player_data.resources;

		const auto it = player_resources.find(resource.type());
		return it != player_resources.end() and it->second >= resource.count();
	}

	auto Resource::require(entt::registry& registry, const std::span<const components::EntityType> types) noexcept -> bool
	{
		const auto resources = entity_type_to_resource<Type::REQUIRE>(types);

		return require(registry, resources);
	}

	auto Resource::require(entt::registry& registry, const components::EntityType type) noexcept -> bool
	{
		const auto resource = entity_type_to_resource<Type::REQUIRE>(type);

		return require(registry, resource);
	}

	auto Resource::consume(entt::registry& registry, const std::span<const components::Resource> resources) noexcept -> bool
	{
		if (not require(registry, resources))
		{
			return false;
		}

		consume_unchecked(registry, resources);

		return true;
	}

	auto Resource::consume(entt::registry& registry, const components::Resource resource) noexcept -> bool
	{
		if (not require(registry, resource))
		{
			return false;
		}

		consume_unchecked(registry, resource);

		return true;
	}

	auto Resource::consume(entt::registry& registry, const std::span<const components::EntityType> types) noexcept -> bool
	{
		const auto resources = entity_type_to_resource<Type::REQUIRE>(types);

		return consume(registry, resources);
	}

	auto Resource::consume(entt::registry& registry, const components::EntityType type) noexcept -> bool
	{
		const auto resource = entity_type_to_resource<Type::REQUIRE>(type);

		return consume(registry, resource);
	}

	auto Resource::consume_unchecked(entt::registry& registry, const std::span<const components::Resource> resources) noexcept -> void
	{
		std::ranges::for_each(
			resources,
			[&](const components::Resource resource) noexcept -> void
			{
				consume_unchecked(registry, resource);
			}
		);
	}

	auto Resource::consume_unchecked(entt::registry& registry, const components::Resource resource) noexcept -> void
	{
		assert(require(registry, resource));

		auto& player_data = registry.ctx().get<components::PlayerData>();
		auto& player_resources = player_data.resources;

		const auto it = player_resources.find(resource.type());
		assert(it->second >= resource.count());
		it->second -= resource.count();
	}

	auto Resource::consume_unchecked(entt::registry& registry, const std::span<const components::EntityType> types) noexcept -> void
	{
		const auto resources = entity_type_to_resource<Type::REQUIRE>(types);

		consume_unchecked(registry, resources);
	}

	auto Resource::consume_unchecked(entt::registry& registry, const components::EntityType type) noexcept -> void
	{
		const auto resource = entity_type_to_resource<Type::REQUIRE>(type);

		consume_unchecked(registry, resource);
	}
}
