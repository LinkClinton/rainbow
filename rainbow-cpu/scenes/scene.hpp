#pragma once

#include "../shared/accelerators/accelerator.hpp"
#include "../interfaces/noncopyable.hpp"
#include "../emitters/emitter.hpp"
#include "../shapes/shape.hpp"
#include "entity.hpp"

#include <memory>
#include <vector>

namespace rainbow::cpus::scenes {

	using namespace accelerators;
	using namespace emitters;
	using namespace shapes;

	class scene : public interfaces::noncopyable {
	public:
		scene() = default;

		~scene() = default;

		void add_entity(const std::shared_ptr<entity>& entity);

		void build_accelerator();

		std::optional<surface_interaction> intersect(const ray& ray) const;

		std::optional<surface_interaction> intersect_with_shadow_ray(const ray& ray) const;

		spectrum evaluate_media_beam(
			const std::shared_ptr<sampler1d>& sampler, const std::tuple<medium_info, interaction>& from, 
			const interaction& to) const;
		
		const std::vector<std::shared_ptr<entity>>& entities() const noexcept;

		const std::vector<std::shared_ptr<entity>>& emitters() const noexcept;

		const std::vector<std::shared_ptr<entity>>& environments() const noexcept;
	private:
		struct entity_reference {
			constexpr static inline size_t all = std::numeric_limits<size_t>::max();

			std::shared_ptr<entity> entity;

			size_t index = all;

			entity_reference() = default;

			entity_reference(const std::shared_ptr<class entity>& entity, size_t index = all);

			std::optional<surface_interaction> intersect(const ray& ray) const;

			bound3 bounding_box() const;

			bool visible() const noexcept;
		};
	private:
		std::vector<std::shared_ptr<entity>> mEntities;
		std::vector<std::shared_ptr<entity>> mEmitters;
		std::vector<std::shared_ptr<entity>> mEnvironments;

		std::shared_ptr<accelerator<entity_reference>> mAccelerator;
	};

}