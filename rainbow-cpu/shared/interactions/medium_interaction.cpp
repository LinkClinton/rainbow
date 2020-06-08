#include "medium_interaction.hpp"

rainbow::cpus::shared::interactions::medium_interaction::medium_interaction(
	const std::shared_ptr<phases::phase_function>& function, const vector3& point, const vector3& wo) :
	interaction(vector3(), point, wo), function(function)
{
}
