#include "scattering_function.hpp"

#include "../../rainbow-core/sample_function.hpp"

rainbow::cpus::scatterings::scattering_type rainbow::cpus::scatterings::operator|(const scattering_type& left,
	const scattering_type& right)
{
	return static_cast<scattering_type>(static_cast<uint32>(left) | static_cast<uint32>(right));
}

rainbow::cpus::scatterings::scattering_type rainbow::cpus::scatterings::operator&(const scattering_type& left,
	const scattering_type& right)
{
	return static_cast<scattering_type>(static_cast<uint32>(left) & static_cast<uint32>(right));
}

rainbow::cpus::scatterings::scattering_type rainbow::cpus::scatterings::operator^(const scattering_type& left,
	const scattering_type& right)
{
	return static_cast<scattering_type>(static_cast<uint32>(left) ^ static_cast<uint32>(right));
}

bool rainbow::cpus::scatterings::match(const scattering_type& target, const scattering_type& flag)
{
	return (target & flag) == flag;
}

bool rainbow::cpus::scatterings::has(const scattering_type& target, const scattering_type& flag)
{
	return (target & flag) == flag;
}

rainbow::cpus::scatterings::scattering_sample::scattering_sample(
	const scattering_type& type, 
	const spectrum& value,
	const vector3& wi, real pdf) :
	type(type), value(value), wi(wi), pdf(pdf)
{
}

rainbow::cpus::scatterings::bidirectional_scattering_distribution_function::bidirectional_scattering_distribution_function(
	const scattering_type& type) : mType(type)
{
}

rainbow::cpus::shared::spectrums::spectrum rainbow::cpus::scatterings::bidirectional_scattering_distribution_function::rho(
	const vector3& wo, const std::vector<vector2>& samples) const
{
	throw std::exception("not implementation.");
}

rainbow::cpus::shared::spectrums::spectrum rainbow::cpus::scatterings::bidirectional_scattering_distribution_function::rho(
	const std::vector<vector2>& sample0, const std::vector<vector2>& sample1) const
{
	throw std::exception("not implementation.");
}

rainbow::cpus::scatterings::scattering_type rainbow::cpus::scatterings::bidirectional_scattering_distribution_function::type() const noexcept
{
	return mType;
}
