#pragma once

#include "../../interfaces/noncopyable.hpp"
#include "../math/math.hpp"
#include "../utilities.hpp"

#include <vector>
#include <memory>

namespace rainbow {

	using namespace math;
	
	namespace distributions {

		template <size_t Dimension>
		struct distribution_sample_t;

		template <>
		struct distribution_sample_t<1> {
			size_t offset = 0;
			real value = 0;
			real pdf = 0;

			distribution_sample_t() = default;

			distribution_sample_t(size_t offset, real value, real pdf);
		};

		template <>
		struct distribution_sample_t<2> {
			vector2 value = vector2(0);
			real pdf = 0;

			distribution_sample_t() = default;

			distribution_sample_t(const vector2& value, real pdf);
		};
		
		template <size_t Dimension>
		class distribution_t;

		template <>
		class distribution_t<1> final : public interfaces::noncopyable {
		public:
			explicit distribution_t(const std::vector<real>& value);

			distribution_sample_t<1> sample(const vector_t<1, real>& sample) const;

			distribution_sample_t<1> sample_discrete(const vector_t<1, real>& sample) const;
			
			real integral() const noexcept;

			real value(size_t index) const noexcept;

			size_t count() const noexcept;
		private:
			size_t find_interval(real x) const;
			
			std::vector<real> mValues;
			std::vector<real> mCDFs;

			real mIntegral;
		};

		template <>
		class distribution_t<2> final : public interfaces::noncopyable {
		public:
			explicit distribution_t(const std::vector<real>& value, size_t width, size_t height);

			distribution_sample_t<2> sample(const vector_t<2, real>& sample) const;

			real pdf(const vector_t<2, real>& point) const;
		private:
			std::vector<std::shared_ptr<distribution_t<1>>> mCDFs;
			std::shared_ptr<distribution_t<1>> mMDFs;

			size_t mWidth, mHeight;
		};

		using distribution1d = distribution_t<1>;
		using distribution2d = distribution_t<2>;
	}	
}

#include "detail/distribution.hpp"