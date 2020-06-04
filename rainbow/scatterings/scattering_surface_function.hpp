#pragma once

#include "scattering_function_collection.hpp"

#include "../materials/material.hpp"
#include "../scenes/scene.hpp"

namespace rainbow {

	using namespace materials;
	using namespace scenes;
	
	namespace scatterings {

		struct scattering_surface_sample {
			scattering_function_collection functions;
			surface_interaction interaction;
			spectrum value = 0;
			real pdf = 0;

			scattering_surface_sample() = default;

			scattering_surface_sample(
				const scattering_function_collection& functions,
				const surface_interaction& interaction,
				const spectrum& value,
				real pdf);
		};
		
		class bidirectional_scattering_surface_distribution_function {
		public:
			explicit bidirectional_scattering_surface_distribution_function(
				const surface_interaction& interaction,
				real eta);

			virtual ~bidirectional_scattering_surface_distribution_function() = default;
			
			virtual scattering_surface_sample sample(const std::shared_ptr<scene>& scene, const vector3& sample) = 0;

			virtual real pdf(const surface_interaction& interaction) = 0;
		protected:
			surface_interaction mInteraction;

			real mEta;
		};

		using scattering_surface_function = bidirectional_scattering_surface_distribution_function;
		
		class separable_bidirectional_scattering_surface_distribution_function : public scattering_surface_function {
		public:
			explicit separable_bidirectional_scattering_surface_distribution_function(
				const surface_interaction& interaction,
				real eta);

			~separable_bidirectional_scattering_surface_distribution_function() = default;

			scattering_surface_sample sample(const std::shared_ptr<scene>& scene, const vector3& sample) override;

			real pdf(const surface_interaction& interaction) override;
		protected:
			virtual spectrum evaluate_reflectance_profile(real distance) = 0;

			virtual real sample_reflectance_profile(size_t channel, real sample) = 0;

			virtual real pdf_reflectance_profile(size_t channel, real distance) = 0;
		protected:
			std::shared_ptr<material> mMaterial;

			coordinate_system mCoordinateSystem;
		};

		using separable_scattering_surface_function = separable_bidirectional_scattering_surface_distribution_function;
	}
}
