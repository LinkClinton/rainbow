#pragma once

#include "texture.hpp"

namespace rainbow::cpus::textures {

	template <size_t Dimension, typename T>
	class image_texture_t;

	template <typename T>
	class image_texture_t<2, T> final : public texture_t<2, T> {
	public:
		explicit image_texture_t(const vector_t<2, size_t>& size, const std::vector<T>& values);

		explicit image_texture_t(const vector_t<2, size_t>& size);

		~image_texture_t() = default;

		void multiply(const T& value) override;

		auto copy_to() const->std::shared_ptr<texture_t<2, T>> override;

		T sample(const surface_interaction& interaction) const override;

		T sample(const vector_t<2, real>& point) const override;

		T value(size_t index) const;
	private:
		std::vector<T> mValues;
	};

	template <typename T>
	class image_texture_t<3, T> final : public texture_t<3, T> {
	public:
		explicit image_texture_t(const vector_t<3, size_t>& size, const std::vector<T>& values);

		explicit image_texture_t(const vector_t<3, size_t>& size);

		~image_texture_t() = default;

		void multiply(const T& value) override;

		auto copy_to() const->std::shared_ptr<texture_t<3, T>> override;

		T sample(const surface_interaction& interaction) const override;

		T sample(const vector_t<3, real>& point) const override;

		T value(size_t index) const;
	private:
		std::vector<T> mValues;
	};

	template <typename T>
	using image_texture2d = image_texture_t<2, T>;

	template <typename T>
	using image_texture3d = image_texture_t<3, T>;

}

#include "detail/image_texture.hpp"