#include <systems/helper/resource.hpp>

#include <components/tags.hpp>
#include <components/player.hpp>

#include <entt/entt.hpp>

namespace
{
	// 塔建造需要的资源和拆除返还的资源不同
	enum class Type : std::uint8_t
	{
		ACQUIRE,
		REQUIRE,
	};

	template<Type T>
	[[nodiscard]] constexpr auto entity_type_to_resource(const components::EntityType type) noexcept -> components::resource::Resource
	{
		using namespace components;

		// todo: 加载配置文件
		std::ignore = type;

		using enum resource::Type;
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
	[[nodiscard]] constexpr auto entity_type_to_resource(const std::span<const components::EntityType> types) noexcept -> std::vector<components::resource::Resource>
	{
		using namespace components;

		std::vector<resource::Resource> resources{};
		resources.reserve(types.size());

		std::ranges::transform(
			types,
			std::back_inserter(resources),
			static_cast<auto(*)(EntityType type) noexcept -> resource::Resource>(entity_type_to_resource<T>)
		);

		return resources;
	}
}

namespace systems::helper
{
	auto Resource::acquire(entt::registry& registry, const std::span<const components::resource::Resource> resources) noexcept -> void
	{
		for (const auto resource: resources)
		{
			acquire(registry, resource);
		}
	}

	auto Resource::acquire(entt::registry& registry, const components::resource::Resource resource) noexcept -> entt::entity
	{
		const auto entity = registry.create();

		registry.emplace<components::resource::Resource>(entity, resource);

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

	auto Resource::require(entt::registry& registry, const std::span<const components::resource::Resource> resources) noexcept -> bool
	{
		return std::ranges::all_of(
			resources,
			[&](const components::resource::Resource resource) noexcept -> bool
			{
				return require(registry, resource);
			}
		);
	}

	auto Resource::require(entt::registry& registry, const components::resource::Resource resource) noexcept -> bool
	{
		using namespace components;

		auto& [player_resources] = registry.ctx().get<player::Resource>();

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

	auto Resource::consume(entt::registry& registry, const std::span<const components::resource::Resource> resources) noexcept -> bool
	{
		if (not require(registry, resources))
		{
			return false;
		}

		consume_unchecked(registry, resources);

		return true;
	}

	auto Resource::consume(entt::registry& registry, const components::resource::Resource resource) noexcept -> bool
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

	auto Resource::consume_unchecked(entt::registry& registry, const std::span<const components::resource::Resource> resources) noexcept -> void
	{
		std::ranges::for_each(
			resources,
			[&](const components::resource::Resource resource) noexcept -> void
			{
				consume_unchecked(registry, resource);
			}
		);
	}

	auto Resource::consume_unchecked(entt::registry& registry, const components::resource::Resource resource) noexcept -> void
	{
		assert(require(registry, resource));

		using namespace components;

		auto& [player_resources] = registry.ctx().get<player::Resource>();

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
