#pragma once

#include "../coefficient_spectrum.hpp"
#include "../math/math.hpp"

namespace rainbow {

	namespace spectrums {

		template <size_t NumSpectrumSamples>
		coefficient_spectrum<NumSpectrumSamples>::coefficient_spectrum(const real& value)
		{
			for (size_t index = 0; index < NumSpectrumSamples; index++)
				coefficient[index] = value;
		}

		template <size_t NumSpectrumSamples>
		coefficient_spectrum<NumSpectrumSamples>::coefficient_spectrum(const std::array<real, NumSpectrumSamples>& samples) :
			coefficient(samples)
		{
		}

		template <size_t NumSpectrumSamples>
		bool coefficient_spectrum<NumSpectrumSamples>::is_black() const noexcept
		{
			for (size_t index = 0; index < NumSpectrumSamples; index++)
				if (coefficient[index] != static_cast<real>(0)) return false;

			return true;
		}

		template <size_t NumSpectrumSamples>
		bool coefficient_spectrum<NumSpectrumSamples>::has_nan() const noexcept
		{
			for (size_t index = 0; index < NumSpectrumSamples; index++)
				if (isnan(coefficient[index])) return true;

			return false;
		}

		template <size_t NumSpectrumSamples>
		real coefficient_spectrum<NumSpectrumSamples>::max_component() const noexcept
		{
			if (NumSpectrumSamples == 0) return 0;

			auto max_value = coefficient[0];

			for (size_t index = 1; index < NumSpectrumSamples; index++)
				max_value = math::max(max_value, coefficient[index]);

			return max_value;
		}

		template <size_t NumSpectrumSamples>
		typename coefficient_spectrum<NumSpectrumSamples>::type& coefficient_spectrum<NumSpectrumSamples>::operator+=(
			const coefficient_spectrum<NumSpectrumSamples>& right) noexcept
		{
			for (size_t index = 0; index < NumSpectrumSamples; index++)
				coefficient[index] += right.coefficient[index];

			return *this;
		}

		template <size_t NumSpectrumSamples>
		typename coefficient_spectrum<NumSpectrumSamples>::type coefficient_spectrum<NumSpectrumSamples>::operator+(
			const coefficient_spectrum<NumSpectrumSamples>& right) const noexcept
		{
			coefficient_spectrum<NumSpectrumSamples> spectrum;

			for (size_t index = 0; index < NumSpectrumSamples; index++)
				spectrum.coefficient[index] = coefficient[index] + right.coefficient[index];

			return spectrum;
		}

		template <size_t NumSpectrumSamples>
		typename coefficient_spectrum<NumSpectrumSamples>::type& coefficient_spectrum<NumSpectrumSamples>::operator-=(
			const coefficient_spectrum<NumSpectrumSamples>& right) noexcept
		{
			for (size_t index = 0; index < NumSpectrumSamples; index++)
				coefficient[index] -= right.coefficient[index];

			return *this;
		}

		template <size_t NumSpectrumSamples>
		typename coefficient_spectrum<NumSpectrumSamples>::type coefficient_spectrum<NumSpectrumSamples>::operator-(
			const coefficient_spectrum<NumSpectrumSamples>& right) const noexcept
		{
			coefficient_spectrum<NumSpectrumSamples> spectrum;

			for (size_t index = 0; index < NumSpectrumSamples; index++)
				spectrum.coefficient[index] = coefficient[index] - right.coefficient[index];

			return spectrum;
		}

		template <size_t NumSpectrumSamples>
		typename coefficient_spectrum<NumSpectrumSamples>::type& coefficient_spectrum<NumSpectrumSamples>::operator*=(
			const coefficient_spectrum<NumSpectrumSamples>& right) noexcept
		{
			for (size_t index = 0; index < NumSpectrumSamples; index++)
				coefficient[index] *= right.coefficient[index];

			return *this;
		}

		template <size_t NumSpectrumSamples>
		typename coefficient_spectrum<NumSpectrumSamples>::type coefficient_spectrum<NumSpectrumSamples>::operator*(
			const coefficient_spectrum<NumSpectrumSamples>& right) const noexcept
		{
			coefficient_spectrum<NumSpectrumSamples> spectrum;

			for (size_t index = 0; index < NumSpectrumSamples; index++)
				spectrum.coefficient[index] = coefficient[index] * right.coefficient[index];

			return spectrum;
		}

		template <size_t NumSpectrumSamples>
		typename coefficient_spectrum<NumSpectrumSamples>::type& coefficient_spectrum<NumSpectrumSamples>::operator/=(
			const coefficient_spectrum<NumSpectrumSamples>& right)
		{
			for (size_t index = 0; index < NumSpectrumSamples; index++) {
				assert(right.coefficient[index] != 0);

				coefficient[index] /= right.coefficient[index];
			}

			return *this;
		}

		template <size_t NumSpectrumSamples>
		typename coefficient_spectrum<NumSpectrumSamples>::type coefficient_spectrum<NumSpectrumSamples>::operator/(
			const coefficient_spectrum<NumSpectrumSamples>& right) const
		{
			coefficient_spectrum<NumSpectrumSamples> spectrum;

			for (size_t index = 0; index < NumSpectrumSamples; index++) {
				assert(right.coefficient[index] != 0);

				spectrum.coefficient[index] = coefficient[index] / right.coefficient[index];
			}

			return spectrum;
		}

		template <size_t NumSpectrumSamples>
		bool coefficient_spectrum<NumSpectrumSamples>::operator==(
			const coefficient_spectrum<NumSpectrumSamples>& right) const noexcept
		{
			for (size_t index = 0; index < NumSpectrumSamples; index++)
				if (coefficient[index] != right.coefficient[index]) return false;

			return true;
		}

		template <size_t NumSpectrumSamples>
		bool coefficient_spectrum<NumSpectrumSamples>::operator!=(
			const coefficient_spectrum<NumSpectrumSamples>& right) const noexcept
		{
			return !operator==(right);
		}

		template <size_t NumSpectrumSamples>
		typename coefficient_spectrum<NumSpectrumSamples>::type& coefficient_spectrum<NumSpectrumSamples>::operator*=(
			const real right) noexcept
		{
			for (size_t index = 0; index < NumSpectrumSamples; index++)
				coefficient[index] *= right;

			return *this;
		}

		template <size_t NumSpectrumSamples>
		typename coefficient_spectrum<NumSpectrumSamples>::type coefficient_spectrum<NumSpectrumSamples>::operator*(
			const real right) const noexcept
		{
			coefficient_spectrum<NumSpectrumSamples> spectrum;

			for (size_t index = 0; index < NumSpectrumSamples; index++)
				spectrum.coefficient[index] = coefficient[index] * right;

			return spectrum;
		}

		template <size_t NumSpectrumSamples>
		typename coefficient_spectrum<NumSpectrumSamples>::type& coefficient_spectrum<NumSpectrumSamples>::operator/=(
			const real right)
		{
			assert(right != 0);

			for (size_t index = 0; index < NumSpectrumSamples; index++)
				coefficient[index] /= right;

			return *this;
		}

		template <size_t NumSpectrumSamples>
		typename coefficient_spectrum<NumSpectrumSamples>::type coefficient_spectrum<NumSpectrumSamples>::operator/(
			const real right) const
		{
			assert(right != 0);

			coefficient_spectrum<NumSpectrumSamples> spectrum;

			for (size_t index = 0; index < NumSpectrumSamples; index++)
				spectrum.coefficient[index] = coefficient[index] / right;

			return spectrum;
		}

		template <size_t NumSpectrumSamples>
		coefficient_spectrum<NumSpectrumSamples> sqrt(const coefficient_spectrum<NumSpectrumSamples>& spectrum)
		{
			coefficient_spectrum<NumSpectrumSamples> result;
			
			for (size_t index = 0; index < NumSpectrumSamples; index++)
				result.coefficient[index] = math::sqrt(spectrum.coefficient[index]);

			return result;
		}

		template <size_t NumSpectrumSamples>
		coefficient_spectrum<NumSpectrumSamples> lerp(
			const coefficient_spectrum<NumSpectrumSamples>& s,
			const coefficient_spectrum<NumSpectrumSamples>& t, const real x)
		{
			return s * (1 - x) + t * x;
		}

	}
}