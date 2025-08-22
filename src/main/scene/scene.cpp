#include <scene/scene.hpp>

#include <entt/entt.hpp>

namespace scene
{
	Scene::Scene(std::shared_ptr<entt::registry> global_registry) noexcept
		: global_registry_{std::move(global_registry)}
	{
		//
	}

	Scene::~Scene() noexcept = default;
}
