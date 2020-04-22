#pragma once

#include "../interfaces/noncopyable.hpp"

#include "../shared/interactions/surface_interaction.hpp"
#include "../shared/transform.hpp"
#include "../shared/ray.hpp"

#include <optional>

namespace rainbow {

	using namespace interactions;
	
	namespace shapes {

		struct shape_sample {
			interaction interaction;
			real pdf = 0;

			shape_sample() = default;

			shape_sample(
				const interactions::interaction& interaction,
				real pdf);

			static shape_sample transform(const transform& transform, const shape_sample& sample);
		};

		class shape : public interfaces::noncopyable, public std::enable_shared_from_this<shape> {
		public:
			shape() = default;

			~shape() = default;
			
			virtual std::optional<surface_interaction> intersect(const ray& ray) const = 0;

			virtual shape_sample sample(const interaction& reference, const vector2& sample) const;

			virtual shape_sample sample(const vector2& sample) const = 0;
			
			virtual real pdf(const interaction& reference, const vector3& wi) const;

			virtual real pdf() const = 0;

			virtual real area() const noexcept = 0;
		}; 
		
	}
}