#include <scene/scene.hpp>

#include <component/registry.hpp>

#include <entt/entt.hpp>

namespace scene
{
	Scene::Scene(std::shared_ptr<entt::registry> global_registry) noexcept
	{
		using namespace global::component;

		scene_registry_.ctx().emplace<registry::Registry>(std::move(global_registry));
	}

	Scene::~Scene() noexcept = default;
}
