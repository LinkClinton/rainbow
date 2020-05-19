#pragma once

#include "../utilities.hpp"
#include "../assert.hpp"

#include <array>

namespace rainbow {

	namespace spectrums {

		template <size_t NumSpectrumSamples>
		struct coefficient_spectrum {
			using type = coefficient_spectrum<NumSpectrumSamples>;

			std::array<real, NumSpectrumSamples> coefficient = {};

			coefficient_spectrum() = default;

			coefficient_spectrum(const real& value);

			coefficient_spectrum(const std::array<real, NumSpectrumSamples>& samples);

			bool is_black() const noexcept;

			bool has_nan() const noexcept;
			
			real max_component() const noexcept;
			
			type& operator+=(const coefficient_spectrum<NumSpectrumSamples>& right) noexcept;
			type operator+(const coefficient_spectrum<NumSpectrumSamples>& right) const noexcept;

			type& operator-=(const coefficient_spectrum<NumSpectrumSamples>& right) noexcept;
			type operator-(const coefficient_spectrum<NumSpectrumSamples>& right) const noexcept;

			type& operator*=(const coefficient_spectrum<NumSpectrumSamples>& right) noexcept;
			type operator*(const coefficient_spectrum<NumSpectrumSamples>& right) const noexcept;

			type& operator/=(const coefficient_spectrum<NumSpectrumSamples>& right);
			type operator/(const coefficient_spectrum<NumSpectrumSamples>& right) const;

			bool operator==(const coefficient_spectrum<NumSpectrumSamples>& right) const noexcept;
			bool operator!=(const coefficient_spectrum<NumSpectrumSamples>& right) const noexcept;

			type& operator*=(const real right) noexcept;
			type operator*(const real right) const noexcept;

			type& operator/=(const real right);
			type operator/(const real right) const;
		};

		template <size_t NumSpectrumSamples>
		coefficient_spectrum<NumSpectrumSamples> sqrt(const coefficient_spectrum<NumSpectrumSamples>& spectrum);

		template <size_t NumSpectrumSamples>
		coefficient_spectrum<NumSpectrumSamples> lerp(
			const coefficient_spectrum<NumSpectrumSamples>& s,
			const coefficient_spectrum<NumSpectrumSamples>& t,
			const real x);

		template <size_t NumSpectrumSamples>
		coefficient_spectrum<NumSpectrumSamples> clamp(
			const coefficient_spectrum<NumSpectrumSamples>& value,
			const coefficient_spectrum<NumSpectrumSamples>& min = coefficient_spectrum<NumSpectrumSamples>(0),
			const coefficient_spectrum<NumSpectrumSamples>& max = coefficient_spectrum<NumSpectrumSamples>(1));
	}
}

#include "detail/coefficient_specturm.hpp"