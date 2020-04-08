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

			explicit scattering_function_collection(
				const std::vector<std::shared_ptr<scattering_function>>& functions);

			~scattering_function_collection() = default;

			void add_scattering_function(const std::shared_ptr<scattering_function>& function);

			spectrum evaluate(const vector3& wo, const vector3& wi) const;
			
			scattering_sample sample(
				const std::shared_ptr<surface_interaction>& interaction,
				const vector2& sample) const;

			spectrum rho(const vector3& wo, const std::vector<vector2>& samples) const;

			spectrum rho(const std::vector<vector2>& samples0, const std::vector<vector2>& samples1) const;

			real pdf(const vector3& wo, const vector3& wi) const;

			size_t size() const noexcept;
		private:
			std::vector<std::shared_ptr<scattering_function>> match(const scattering_type& type) const noexcept;
		private:
			std::vector<std::shared_ptr<scattering_function>> mScatteringFunctions;
		};
		
	}
}
