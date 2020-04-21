#pragma once

#include "math/math.hpp"

namespace rainbow {

	using namespace math;

	vector3 uniform_sample_hemisphere(const vector2& sample);

	vector3 cosine_sample_hemisphere(const vector2& sample);

	vector2 concentric_sample_disk(const vector2& sample);

	vector3 uniform_sample_sphere(const vector2& sample);
	
	real uniform_sample_hemisphere_pdf();

	real cosine_sample_hemisphere_pdf(real cos_theta);

}
