#pragma once

#include "../shared/interactions/surface_interaction.hpp"
#include "../interfaces/noncopyable.hpp"

#include "scattering_function.hpp"

#include <vector>
#include <memory>

namespace rainbow {

	using namespace interactions;
	
	namespace scatterings {

		class scattering_function_collection final {
		public:
			scattering_function_collection() = default;
			
			explicit scattering_function_collection(real eta);

			explicit scattering_function_collection(
				const std::vector<std::shared_ptr<scattering_function>>& functions, real eta);

			explicit scattering_function_collection(
				const scattering_function_collection& functions0,
				const scattering_function_collection& functions1);
			
			~scattering_function_collection() = default;

			void add_scattering_function(const std::shared_ptr<scattering_function>& function);

			spectrum evaluate(const vector3& wo, const vector3& wi, 
				const scattering_type& include = scattering_type::all) const;
			
			scattering_sample sample(
				const surface_interaction& interaction,
				const vector2& sample,
				const scattering_type& include = scattering_type::all) const;

			spectrum rho(const vector3& wo, const std::vector<vector2>& samples, 
				const scattering_type& include = scattering_type::all) const;

			spectrum rho(
				const std::vector<vector2>& samples0, 
				const std::vector<vector2>& samples1, 
				const scattering_type& include = scattering_type::all) const;

			real pdf(const vector3& wo, const vector3& wi, const scattering_type& include = scattering_type::all) const;

			size_t count(const scattering_type& include) const noexcept;
			
			size_t count() const noexcept;

			real eta() const noexcept;
		private:
			std::vector<std::shared_ptr<scattering_function>> match(const scattering_type& include) const noexcept;
		private:
			std::vector<std::shared_ptr<scattering_function>> mScatteringFunctions;

			real mEta = 1;
		};
		
	}
}
