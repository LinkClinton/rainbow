#pragma once

#include "../scatterings/scattering_function.hpp"

#include "../interfaces/noncopyable.hpp"

#include <vector>
#include <memory>

namespace rainbow {

	using namespace scatterings;
	
	namespace materials {

		class material : public interfaces::noncopyable {
		public:
			material() = default;
		protected:
			
		};
		
	}
}
