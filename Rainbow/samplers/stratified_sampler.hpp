#pragma once

#include "samplers.hpp"

namespace rainbow {

	namespace samplers {

		template <size_t Dimension>
		class stratified_sampler_t final : public sampler_t<Dimension> {
		public:
			explicit stratified_sampler_t(size_t samples_per_pixel_x, size_t samples_per_pixel_y, size_t dimension);

			explicit stratified_sampler_t(size_t samples_per_pixel_x, size_t samples_per_pixel_y, size_t dimension, size_t seed);

			explicit stratified_sampler_t(size_t samples_per_pixel_x, size_t samples_per_pixel_y, size_t dimension,
				const std::shared_ptr<random_generator>& generator);
			
			std::shared_ptr<sampler_t<Dimension>> clone(size_t seed) const override;

			std::shared_ptr<sampler_t<Dimension>> clone(const std::shared_ptr<random_generator>& generator) const override;

			typename sampler_t<Dimension>::sample_type next() override;

			void next_sample() override;

			void reset() override;
		private:
			using samples = std::vector<typename sampler_t<Dimension>::sample_type>;

			void stratified_sample(samples& samples);

			void shuffle(samples& samples);
			
			std::vector<samples> mSamples;

			size_t mCurrentDimension;
			size_t mSamplesPerPixelX;
			size_t mSamplesPerPixelY;
		};

		using stratified_sampler1d = stratified_sampler_t<1>;
		using stratified_sampler2d = stratified_sampler_t<2>;
	}
}

#include "detail/stratified_sampler.hpp"