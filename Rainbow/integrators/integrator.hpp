#pragma once

#include "../interfaces/noncopyable.hpp"

#include "../shared/spectrums/spectrum.hpp"
#include "../samplers/samplers.hpp"
#include "../cameras/camera.hpp"
#include "../scenes/scene.hpp"

namespace rainbow {

	using namespace spectrums;
	using namespace samplers;
	using namespace cameras;
	using namespace scenes;
	
	namespace integrators {

		class integrator : public interfaces::noncopyable {
		public:
			integrator() = default;

			~integrator() = default;

			virtual void render(
				const std::shared_ptr<camera>& camera,
				const std::shared_ptr<scene>& scene) = 0;
		};

		struct sampler_group {
			std::shared_ptr<sampler1d> sampler1d;
			std::shared_ptr<sampler2d> sampler2d;

			sampler_group() = default;

			sampler_group(
				const std::shared_ptr<samplers::sampler1d>& sampler1d,
				const std::shared_ptr<samplers::sampler2d>& sampler2d);
		};
		
		class sampler_integrator : public integrator {
		public:
			explicit sampler_integrator(
				const std::shared_ptr<sampler2d>& camera_sampler,
				size_t max_depth = 5);

			~sampler_integrator() = default;
			
			void render(
				const std::shared_ptr<camera>& camera, 
				const std::shared_ptr<scene>& scene) override;

			virtual spectrum trace(
				const std::shared_ptr<scene>& scene, const sampler_group& samplers, const ray& ray, size_t depth) = 0;
		protected:
			virtual sampler_group prepare_samplers();
			
			const size_t mMaxDepth = 5;
		private:
			std::shared_ptr<sampler2d> mCameraSampler;
		};
		
	}
}
