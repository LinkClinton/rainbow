#include "curve.hpp"

using namespace rainbow::cpus::shared::interactions;

namespace rainbow::cpus::shapes {

	inline vector3 blossom_bezier_curve(
		const std::array<vector3, 4>& control_points, const std::array<real, 3>& u)
	{
		std::array<vector3, 3> a = {
			lerp(control_points[0], control_points[1], u[0]),
			lerp(control_points[1], control_points[2], u[0]),
			lerp(control_points[2], control_points[3], u[0])
		};

		std::array<vector3, 2> b = {
			lerp(a[0], a[1], u[1]),
			lerp(a[1], a[2], u[1])
		};

		return lerp(b[0], b[1], u[2]);
	}

	inline std::array<vector3, 7> subdivide_bezier_curve(const std::array<vector3, 4>& control_points)
	{
		constexpr real two = 2;
		constexpr real three = 3;
		
		return {
			control_points[0],
			(control_points[0] + control_points[1]) / static_cast<real>(2),
			(control_points[0] + two * control_points[1] + control_points[2]) / static_cast<real>(4),
			(control_points[0] + three * control_points[1] + three * control_points[2] + control_points[3]) / static_cast<real>(8),
			(control_points[1] + two * control_points[2] + control_points[3]) / static_cast<real>(4),
			(control_points[2] + control_points[3]) / static_cast<real>(2),
			control_points[3]
		};
	}

	inline std::tuple<vector3, vector3> evaluate_bezier_curve(
		const std::array<vector3, 4>& control_points, real u)
	{
		std::array<vector3, 3> a = {
			lerp(control_points[0], control_points[1], u),
			lerp(control_points[1], control_points[2], u),
			lerp(control_points[2], control_points[3], u)
		};

		std::array<vector3, 2> b = {
			lerp(a[0], a[1], u),
			lerp(a[1], a[2], u)
		};

		const auto point = lerp(b[0], b[1], u);

		if (length_squared(b[1] - b[0]) > 0)
			return { point, static_cast<real>(3) * (b[1] - b[0]) };
		else
			return { point, control_points[3] - control_points[0] };
	}

	inline real max_component_in_array(const std::array<vector3, 4>& points, int dimension)
	{
		return max(
			max(points[0][dimension], points[1][dimension]), 
			max(points[2][dimension], points[3][dimension]));
	}

	inline real min_component_in_array(const std::array<vector3, 4>& points, int dimension)
	{
		return min(
			min(points[0][dimension], points[1][dimension]),
			min(points[2][dimension], points[3][dimension]));
	}
	
	inline bool intersect_in_ray_space(const std::array<vector3, 4>& control_points, real max_width, real length)
	{
		if (max_component_in_array(control_points, 0) + 0.5 * max_width < 0 ||
			min_component_in_array(control_points, 0) - 0.5 * max_width > 0)
			return false;

		if (max_component_in_array(control_points, 1) + 0.5 * max_width < 0 ||
			min_component_in_array(control_points, 1) - 0.5 * max_width > 0)
			return false;

		if (max_component_in_array(control_points, 2) + 0.5 * max_width < 0 ||
			min_component_in_array(control_points, 2) - 0.5 * max_width > length)
			return false;

		return true;
	}
}

rainbow::cpus::shapes::curve::curve(
	const std::array<vector3, 4>& control_points, 
	const std::array<real, 2>& width,
	real u_min, real u_max) :
	mControlPoints(control_points), mWidth(width), mUMin(u_min), mUMax(u_max)
{
}

std::optional<surface_interaction> rainbow::cpus::shapes::curve::intersect(const ray& ray, size_t index) const
{
	return intersect(ray);
}

std::optional<surface_interaction> rainbow::cpus::shapes::curve::intersect(const ray& ray) const
{
	std::array<vector3, 4> points = {
		blossom_bezier_curve(mControlPoints, { mUMin, mUMin, mUMin }),
		blossom_bezier_curve(mControlPoints, { mUMin, mUMin, mUMax }),
		blossom_bezier_curve(mControlPoints, { mUMin, mUMax, mUMax }),
		blossom_bezier_curve(mControlPoints, { mUMax, mUMax, mUMax }),
	};

	auto dx = math::cross(ray.direction, points[3] - points[0]);

	if (length_squared(dx) == 0) dx = coordinate_system(ray.direction).x();

	const auto local_to_ray = look_at_right_hand(ray.origin, ray.origin + ray.direction, dx);

	points[0] = transform_point(local_to_ray, points[0]);
	points[1] = transform_point(local_to_ray, points[1]);
	points[2] = transform_point(local_to_ray, points[2]);
	points[3] = transform_point(local_to_ray, points[3]);

	const auto max_width = max(
		lerp(mWidth[0], mWidth[1], mUMin), 
		lerp(mWidth[0], mWidth[1], mUMax));

	if (!intersect_in_ray_space(points, max_width, ray.length)) 
		return std::nullopt;

	return recursive_intersect(points, ray, local_to_ray.inverse(), mUMin, mUMax, 10);
}

bound3 rainbow::cpus::shapes::curve::bounding_box(const transform& transform, size_t index) const
{
	return bounding_box(transform);
}

bound3 rainbow::cpus::shapes::curve::bounding_box(const transform& transform) const
{
	std::array<vector3, 4> points = {
		blossom_bezier_curve(mControlPoints, { mUMin, mUMin, mUMin }),
		blossom_bezier_curve(mControlPoints, { mUMin, mUMin, mUMax }),
		blossom_bezier_curve(mControlPoints, { mUMin, mUMax, mUMax }),
		blossom_bezier_curve(mControlPoints, { mUMax, mUMax, mUMax }),
	};

	auto ret = bound3(transform_point(transform, points[0]), transform_point(transform, points[1]));

	ret.union_it(transform_point(transform, points[2]));
	ret.union_it(transform_point(transform, points[3]));

	const auto width0 = lerp(mWidth[0], mWidth[1], mUMin);
	const auto width1 = lerp(mWidth[0], mWidth[1], mUMax);
	const auto max_width = transform_vector(transform, vector3(max(width0, width1)));

	return bound3(ret.min - max_width, ret.max + max_width);
}

rainbow::cpus::shapes::shape_sample rainbow::cpus::shapes::curve::sample(const vector2& sample) const
{
	throw std::exception();
}

real rainbow::cpus::shapes::curve::pdf() const
{
	throw std::exception();
}

real rainbow::cpus::shapes::curve::area(size_t index) const noexcept
{
	return area();
}

real rainbow::cpus::shapes::curve::area() const noexcept
{
	std::array<vector3, 4> points = {
		blossom_bezier_curve(mControlPoints, { mUMin, mUMin, mUMin }),
		blossom_bezier_curve(mControlPoints, { mUMin, mUMin, mUMax }),
		blossom_bezier_curve(mControlPoints, { mUMin, mUMax, mUMax }),
		blossom_bezier_curve(mControlPoints, { mUMax, mUMax, mUMax }),
	};

	const auto width0 = lerp(mWidth[0], mWidth[1], mUMin);
	const auto width1 = lerp(mWidth[0], mWidth[1], mUMax);
	const auto width = (width0 + width1) * 0.5f;

	real length = 0;

	for (size_t index = 0; index < 3; index++)
		length = length + distance(points[index + 0], points[index + 1]);

	return length * width;
}

void rainbow::cpus::shapes::curve::build_accelerator()
{
}

std::optional<surface_interaction> rainbow::cpus::shapes::curve::recursive_intersect(
	const std::array<vector3, 4>& control_points, const ray& ray,
	const transform& ray_to_local, real u_min, real u_max, size_t depth) const
{
	if (depth > 0) {
		const auto points = subdivide_bezier_curve(control_points);

		std::array<real, 3> u = { u_min, (u_min + u_max) * 0.5f, u_max };

		std::optional<surface_interaction> nearest_interaction = std::nullopt;
		
		// loop the segments of curve(divide them into two part)
		for (size_t index = 0; index < 2; index++) {
			// find the max_width of this segment of curve
			const auto max_width = max(
				lerp(mWidth[0], mWidth[1], u[index + 0]),
				lerp(mWidth[0], mWidth[1], u[index + 1]));

			std::array<vector3, 4> sub_points = {
				points[index * 3 + 0], points[index * 3 + 1],
				points[index * 3 + 2], points[index * 3 + 3]
			};

			// if the ray is not intersect with curve segment we just continue
			if (!intersect_in_ray_space(sub_points, max_width, ray.length))
				continue;

			const auto interaction = recursive_intersect(sub_points, ray, ray_to_local,
				u[index + 0], u[index + 1], depth - 1);

			if (interaction.has_value()) nearest_interaction = interaction;
		}

		return nearest_interaction;
	}

	// the case depth = 0

	const auto edge_function0 = 
		(control_points[1].y - control_points[0].y) * -control_points[0].y + control_points[0].x * (control_points[0].x - control_points[1].x);
	const auto edge_function1 =
		(control_points[2].y - control_points[3].y) * -control_points[3].y + control_points[3].x * (control_points[3].x - control_points[2].x);

	if (edge_function0 < 0 || edge_function1 < 0) return std::nullopt;

	const auto direction = vector2(control_points[3]) - vector2(control_points[0]);
	const auto denominator = length_squared(direction);

	if (denominator == 0) return std::nullopt;

	const auto w = dot(-vector2(control_points[0]), direction) / denominator;
	const auto u = clamp(lerp(u_min, u_max, w), u_min, u_max);

	const auto width = lerp(mWidth[0], mWidth[1], u);

	const auto [point, dp_dw] = 
		evaluate_bezier_curve(control_points, clamp(w, static_cast<real>(0), static_cast<real>(1)));

	const auto distance2 = point.x * point.x + point.y * point.y;

	if (distance2 > width * width * 0.25 || point.z < 0 || point.z > ray.length) 
		return std::nullopt;

	const auto distance = sqrt(distance2);
	const auto edge_function = point.x * dp_dw.y - point.y * dp_dw.x;
	const auto v = (edge_function > 0) ? static_cast<real>(0.5) + distance / width : static_cast<real>(0.5) - distance / width;

	const auto [unused, dp_du] = evaluate_bezier_curve(mControlPoints, u);

	const auto dp_du_plane = transform_vector(ray_to_local.inverse(), dp_du);
	const auto dp_dv_plane = normalize(vector3(-dp_du_plane.y, dp_du_plane.x, 0)) * width;

	const auto theta = lerp(static_cast<real>(-90), static_cast<real>(90), v);
	const auto rotate = shared::rotate(-theta, dp_du_plane);

	const auto dp_dv = transform_vector(ray_to_local, transform_vector(rotate, dp_dv_plane));
	const auto normal =
		reverse_orientation() ? -normalize(math::cross(dp_du, dp_dv)) : normalize(math::cross(dp_du, dp_dv));

	// t = point.z
	ray.length = point.z;
	
	return surface_interaction(
		nullptr,
		dp_du, dp_dv, normal, ray.origin + ray.direction * ray.length, -ray.direction, vector2(u, v));
}
