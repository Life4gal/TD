#include <system/game/helper/resource.hpp>

#include <algorithm>
#include <ranges>

#include <component/game/player.hpp>

#include <entt/entt.hpp>

namespace
{
	using namespace game;

	// 塔建造需要的资源和拆除返还的资源不同
	enum class Type : std::uint8_t
	{
		ACQUIRE,
		REQUIRE,
	};

	template<Type T>
	constexpr auto entity_type_to_resource(const component::entity::Type type, std::vector<component::resource::Resource>& out) noexcept -> void
	{
		using namespace component;

		// todo: 加载配置文件
		std::ignore = type;

		if constexpr (T == Type::ACQUIRE)
		{
			out.emplace_back(resource::Type::GOLD, 50);
		}
		else if (T == Type::REQUIRE)
		{
			out.emplace_back(resource::Type::HEALTH, 1);
			out.emplace_back(resource::Type::GOLD, 100);
		}
		else
		{
			std::unreachable();
		}
	}

	template<Type T>
	[[nodiscard]] constexpr auto entity_type_to_resource(const component::entity::Type type) noexcept -> std::vector<component::resource::Resource>
	{
		std::vector<component::resource::Resource> resources{};

		entity_type_to_resource<T>(type, resources);

		return resources;
	}

	template<Type T>
	constexpr auto entity_type_to_resource(const std::span<const component::entity::Type> types, std::vector<component::resource::Resource>& out) noexcept -> void
	{
		using namespace component;

		// 1. 获取所有结果
		std::ranges::for_each(
			types,
			[&](const entity::Type type) noexcept -> void
			{
				entity_type_to_resource<T>(type, out);
			}
		);

		// 2.排序结果
		std::ranges::sort(
			out,
			[](const resource::Type lhs, const resource::Type rhs) noexcept -> bool
			{
				return std::to_underlying(lhs) < std::to_underlying(rhs);
			},
			&resource::Resource::type
		);

		auto v = out | std::views::chunk_by(
			         [](const resource::Resource& lhs, const resource::Resource& rhs) noexcept -> bool
			         {
				         return lhs.type() != rhs.type();
			         }
		         );

		// 3.合并同类型资源
		out =
				out |
				std::views::chunk_by(
					[](const resource::Resource& lhs, const resource::Resource& rhs) noexcept -> bool
					{
						return lhs.type() != rhs.type();
					}
				) |
				std::views::transform(
					[](auto chunk) noexcept -> resource::Resource
					{
						const auto type = chunk.begin()->type();
						const auto total_amount = std::ranges::fold_left(
							chunk,
							resource::size_type{0},
							[](const resource::size_type total, const resource::Resource& resource) noexcept -> resource::size_type
							{
								return total + resource.amount();
							}
						);

						return {type, total_amount};
					}
				) |
				std::ranges::to<std::vector<resource::Resource>>();
	}

	template<Type T>
	[[nodiscard]] constexpr auto entity_type_to_resource(const std::span<const component::entity::Type> types) noexcept -> std::vector<component::resource::Resource>
	{
		std::vector<component::resource::Resource> resources{};

		entity_type_to_resource<T>(types, resources);

		return resources;
	}

	template<Type T>
	constexpr auto entity_to_resource(entt::registry& registry, const entt::entity entity, std::vector<component::resource::Resource>& out) noexcept -> void
	{
		using namespace component;

		const auto type = registry.get<const entity::Type>(entity);

		return entity_type_to_resource<T>(type, out);
	}

	template<Type T>
	[[nodiscard]] constexpr auto entity_to_resource(entt::registry& registry, const entt::entity entity) noexcept -> std::vector<component::resource::Resource>
	{
		using namespace component;

		std::vector<resource::Resource> resources{};

		entity_to_resource<T>(registry, entity, resources);

		return resources;
	}

	template<Type T>
	constexpr auto entity_to_resource(entt::registry& registry, const std::span<const entt::entity> entities, std::vector<component::resource::Resource>& out) noexcept -> void
	{
		using namespace component;

		std::ranges::for_each(
			entities,
			[&](const entt::entity entity) noexcept -> void
			{
				entity_to_resource<T>(registry, entity, out);
			}
		);
	}

	template<Type T>
	[[nodiscard]] constexpr auto entity_to_resource(entt::registry& registry, const std::span<const entt::entity> entities) noexcept -> std::vector<component::resource::Resource>
	{
		std::vector<component::resource::Resource> resources{};

		entity_to_resource<T>(registry, entities, resources);

		return resources;
	}
}

namespace game::system::helper
{
	auto Resource::acquire(entt::registry& registry, const std::span<const resource_type> resources) noexcept -> void
	{
		using namespace component;

		std::vector<entt::entity> entities{};
		entities.resize(resources.size());

		registry.create(entities.begin(), entities.end());

		for (const auto [entity, resource]: std::views::zip(entities, resources))
		{
			registry.emplace<resource_type>(entity, resource);
		}
	}

	auto Resource::acquire(entt::registry& registry, const resource_type resource) noexcept -> void
	{
		using namespace component;

		const auto entity = registry.create();

		registry.emplace<resource_type>(entity, resource);
	}

	auto Resource::acquire(entt::registry& registry, const std::span<const component::entity::Type> types) noexcept -> void
	{
		const auto resources = entity_type_to_resource<Type::ACQUIRE>(types);

		return acquire(registry, resources);
	}

	auto Resource::acquire(entt::registry& registry, const component::entity::Type type) noexcept -> void
	{
		const auto resource = entity_type_to_resource<Type::ACQUIRE>(type);

		acquire(registry, resource);
	}

	auto Resource::acquire(entt::registry& registry, const std::span<const entt::entity> entities) noexcept -> void
	{
		const auto resources = entity_to_resource<Type::ACQUIRE>(registry, entities);

		return acquire(registry, resources);
	}

	auto Resource::acquire(entt::registry& registry, const entt::entity entity) noexcept -> void
	{
		const auto resource = entity_to_resource<Type::ACQUIRE>(registry, entity);

		acquire(registry, resource);
	}

	auto Resource::require(entt::registry& registry, const std::span<const resource_type> resources) noexcept -> bool
	{
		using namespace component;

		const auto& [player_resources] = registry.ctx().get<const player::Resource>();

		return std::ranges::all_of(
			resources,
			[&](const resource_type resource) noexcept -> bool
			{
				const auto player_resource_it = player_resources.find(resource.type());
				return player_resource_it != player_resources.end() and player_resource_it->second >= resource.amount();
			}
		);
	}

	auto Resource::require(entt::registry& registry, const resource_type resource) noexcept -> bool
	{
		using namespace component;

		const auto& [player_resources] = registry.ctx().get<const player::Resource>();

		const auto player_resource_it = player_resources.find(resource.type());
		return player_resource_it != player_resources.end() and player_resource_it->second >= resource.amount();
	}

	auto Resource::require(entt::registry& registry, const std::span<const component::entity::Type> types) noexcept -> bool
	{
		const auto resources = entity_type_to_resource<Type::REQUIRE>(types);

		return require(registry, resources);
	}

	auto Resource::require(entt::registry& registry, const component::entity::Type type) noexcept -> bool
	{
		const auto resource = entity_type_to_resource<Type::REQUIRE>(type);

		return require(registry, resource);
	}

	auto Resource::require(entt::registry& registry, const std::span<const entt::entity> entities) noexcept -> bool
	{
		const auto resources = entity_to_resource<Type::REQUIRE>(registry, entities);

		return require(registry, resources);
	}

	auto Resource::require(entt::registry& registry, const entt::entity entity) noexcept -> bool
	{
		const auto resource = entity_to_resource<Type::REQUIRE>(registry, entity);

		return require(registry, resource);
	}

	auto Resource::consume(entt::registry& registry, const std::span<const resource_type> resources) noexcept -> bool
	{
		if (not require(registry, resources))
		{
			return false;
		}

		consume_unchecked(registry, resources);

		return true;
	}

	auto Resource::consume(entt::registry& registry, const resource_type resource) noexcept -> bool
	{
		if (not require(registry, resource))
		{
			return false;
		}

		consume_unchecked(registry, resource);

		return true;
	}

	auto Resource::consume(entt::registry& registry, const std::span<const component::entity::Type> types) noexcept -> bool
	{
		const auto resources = entity_type_to_resource<Type::REQUIRE>(types);

		return consume(registry, resources);
	}

	auto Resource::consume(entt::registry& registry, const component::entity::Type type) noexcept -> bool
	{
		const auto resource = entity_type_to_resource<Type::REQUIRE>(type);

		return consume(registry, resource);
	}

	auto Resource::consume(entt::registry& registry, const std::span<const entt::entity> entities) noexcept -> bool
	{
		const auto resources = entity_to_resource<Type::REQUIRE>(registry, entities);

		return consume(registry, resources);
	}

	auto Resource::consume(entt::registry& registry, const entt::entity entity) noexcept -> bool
	{
		const auto resource = entity_to_resource<Type::REQUIRE>(registry, entity);

		return consume(registry, resource);
	}

	auto Resource::consume_unchecked(entt::registry& registry, const std::span<const resource_type> resources) noexcept -> void
	{
		using namespace component;

		assert(require(registry, resources));

		auto& [player_resources] = registry.ctx().get<player::Resource>();

		std::ranges::for_each(
			resources,
			[&](const resource_type resource) noexcept -> void
			{
				const auto player_resource_it = player_resources.find(resource.type());
				assert(player_resource_it != player_resources.end());

				player_resource_it->second -= resource.amount();
			}
		);
	}

	auto Resource::consume_unchecked(entt::registry& registry, const resource_type resource) noexcept -> void
	{
		using namespace component;

		assert(require(registry, resource));

		auto& [player_resources] = registry.ctx().get<player::Resource>();

		const auto player_resource_it = player_resources.find(resource.type());
		assert(player_resource_it != player_resources.end());

		player_resource_it->second -= resource.amount();
	}

	auto Resource::consume_unchecked(entt::registry& registry, const std::span<const component::entity::Type> types) noexcept -> void
	{
		const auto resources = entity_type_to_resource<Type::REQUIRE>(types);

		consume_unchecked(registry, resources);
	}

	auto Resource::consume_unchecked(entt::registry& registry, const component::entity::Type type) noexcept -> void
	{
		const auto resource = entity_type_to_resource<Type::REQUIRE>(type);

		consume_unchecked(registry, resource);
	}

	auto Resource::consume_unchecked(entt::registry& registry, const std::span<const entt::entity> entities) noexcept -> void
	{
		const auto resources = entity_to_resource<Type::REQUIRE>(registry, entities);

		consume_unchecked(registry, resources);
	}

	auto Resource::consume_unchecked(entt::registry& registry, const entt::entity entity) noexcept -> void
	{
		const auto resource = entity_to_resource<Type::REQUIRE>(registry, entity);

		consume_unchecked(registry, resource);
	}
}
