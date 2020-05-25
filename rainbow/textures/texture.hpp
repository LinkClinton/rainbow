#pragma once

#include "../interfaces/noncopyable.hpp"

#include "../shared/interactions/surface_interaction.hpp"
#include "../shared/math/math.hpp"

#include <vector>

namespace rainbow {

	using namespace interactions;
	using namespace math;
	
	namespace textures {

		template <size_t Dimension>
		using texture_size = vector_t<Dimension, size_t>;

		template <size_t Dimension>
		class texture_region_t : public interfaces::noncopyable {
		public:
			explicit texture_region_t(const texture_size<Dimension>& size);

			~texture_region_t() = default;

			texture_size<Dimension> size() const noexcept;

			size_t index(const vector_t<Dimension, size_t>& location) const;

			size_t length() const noexcept;
		protected:
			vector_t<Dimension, size_t> mSize;
		};

		template <typename T>
		class texture_sample_t : public interfaces::noncopyable {
		public:
			texture_sample_t() = default;

			~texture_sample_t() = default;

			virtual T sample(const surface_interaction& interaction) const = 0;

			virtual T sample(const vector2& point) const = 0;
		};
	
		template <size_t Dimension, typename T>
		class texture_t : public texture_region_t<Dimension>, public texture_sample_t<T>,
			public std::enable_shared_from_this<texture_t<Dimension, T>> {
		public:
			explicit texture_t(const vector_t<Dimension, size_t>& size);

			virtual void multiply(const T& value) = 0;

			virtual auto copy_to() const -> std::shared_ptr<texture_t<Dimension, T>> = 0;
			
			~texture_t() = default;
		};
		
		template <typename T>
		using texture2d = texture_t<2, T>;
		
	}
}

#include "detail/texture.hpp"