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
	real u_min, real u_max,
	bool reverse_orientation) : shape(reverse_orientation),
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

	// build the transform from ray space to local space, we use left hand(the positive z-axis is the ray)
	// we also need a vector called up, we use dx = cross(ray.direction, points[3] - points[0])
	// if the dx is point, we will use coordinate_system to build a dx.
	const auto local_to_ray = look_at_left_hand(ray.origin, ray.origin + ray.direction, dx);

	// transform points from local space to ray space
	points[0] = transform_point(local_to_ray, points[0]);
	points[1] = transform_point(local_to_ray, points[1]);
	points[2] = transform_point(local_to_ray, points[2]);
	points[3] = transform_point(local_to_ray, points[3]);

	// evaluate the max width of curve
	const auto max_width = max(
		lerp(mWidth[0], mWidth[1], mUMin), 
		lerp(mWidth[0], mWidth[1], mUMax));

	// the bounding box in ray space, so we only need the length of ray(the origin is (0, 0, 0), the direction is (0, 0, 1))
	// if the bounding box is not intersect with the ray, we return std::nullopt
	if (!intersect_in_ray_space(points, max_width, ray.length)) 
		return std::nullopt;

	return recursive_intersect(points, ray, local_to_ray.inverse(), mUMin, mUMax, 5);
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

	// find the max width of curve, we will use it to expand the bounding box
	// the bounding box with width is (min - width, max + width)
	const auto width0 = lerp(mWidth[0], mWidth[1], mUMin);
	const auto width1 = lerp(mWidth[0], mWidth[1], mUMax);
	const auto max_width = transform_vector(transform, vector3(max(width0, width1)));

	return bound3(ret.min - max_width, ret.max + max_width);
}

rainbow::cpus::shapes::shape_sample rainbow::cpus::shapes::curve::sample(const shape_instance_properties& properties, const vector2& sample) const
{
	// not support in this version
	throw std::exception();
}

real rainbow::cpus::shapes::curve::pdf(const shape_instance_properties& properties) const
{
	// not support in this version
	throw std::exception();
}

real rainbow::cpus::shapes::curve::area(const transform& transform, size_t index) const noexcept
{
	return area(transform);
}

real rainbow::cpus::shapes::curve::area(const transform& transform) const noexcept
{
	const std::array<vector3, 4> transformed_points = {
		transform_point(transform, mControlPoints[0]),
		transform_point(transform, mControlPoints[1]),
		transform_point(transform, mControlPoints[2]),
		transform_point(transform, mControlPoints[3])
	};
	
	std::array<vector3, 4> points = {
		blossom_bezier_curve(transformed_points, { mUMin, mUMin, mUMin }),
		blossom_bezier_curve(transformed_points, { mUMin, mUMin, mUMax }),
		blossom_bezier_curve(transformed_points, { mUMin, mUMax, mUMax }),
		blossom_bezier_curve(transformed_points, { mUMax, mUMax, mUMax }),
	};

	// evaluate the average of width, we use it as the width of curve in area calculation
	const auto width0 = lerp(mWidth[0], mWidth[1], mUMin);
	const auto width1 = lerp(mWidth[0], mWidth[1], mUMax);
	const auto width = (width0 + width1) * 0.5f;

	real length = 0;

	// the length of curve, we use the length of three segment in area calculation
	for (size_t index = 0; index < 3; index++)
		length = length + distance(points[index + 0], points[index + 1]);

	// the approximate area should be the length * width
	return length * width;
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

	// evaluate the average of width, we use it as the width of curve in area calculation
	const auto width0 = lerp(mWidth[0], mWidth[1], mUMin);
	const auto width1 = lerp(mWidth[0], mWidth[1], mUMax);
	const auto width = (width0 + width1) * 0.5f;

	real length = 0;

	// the length of curve, we use the length of three segment in area calculation
	for (size_t index = 0; index < 3; index++)
		length = length + distance(points[index + 0], points[index + 1]);

	// the approximate area should be the length * width
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

			// now, intersect the sub-curve
			const auto interaction = recursive_intersect(sub_points, ray, ray_to_local,
				u[index + 0], u[index + 1], depth - 1);

			if (interaction.has_value()) nearest_interaction = interaction;
		}

		return nearest_interaction;
	}

	// the case depth = 0

	// the edge_function0 is the edge_function of begin
	// the edge_function1 is the edge_function of end
	// we use edge_function to test the side of the point(0, 0)
	// we project the curve to the x-y plane(just ignore the z value)
	// in fact, edge_function is the dot of tangent at begin/end and vector from point to begin/end
	const auto edge_function0 = 
		(control_points[1].y - control_points[0].y) * -control_points[0].y + control_points[0].x * (control_points[0].x - control_points[1].x);
	const auto edge_function1 =
		(control_points[2].y - control_points[3].y) * -control_points[3].y + control_points[3].x * (control_points[3].x - control_points[2].x);

	if (edge_function0 < 0 || edge_function1 < 0) return std::nullopt;

	// now, we will assume the curve is a segment from begin to end point
	const auto direction = vector2(control_points[3]) - vector2(control_points[0]);
	const auto denominator = length_squared(direction);

	if (denominator == 0) return std::nullopt;

	// w is the factor of segment, when w = 0, the point should be begin, when w = 1, the point should be end(parametric form)
	// the distance of begin point and the projection of (0, 0) in vector(points[3] - points[0]) should be
	// length((0, 0) - points[0]) * cos theta, where theta is the angle between the vector((0, 0) - points[0]) and vector (points[3] - points[0])
	// cos theta = ((0, 0) - points[0]) dot (points[3] - points[0]) / (length((0,0) - points[0]) * length(points[3] - points[0]))
	// so the distance = ((0, 0) - points[0]) dot (points[3] - points[0]) / length(points[3] - points[0])
	// so the w = distance / length(points[3] - points[0])
	const auto w = dot(-vector2(control_points[0]), direction) / denominator;
	const auto u = clamp(lerp(u_min, u_max, w), u_min, u_max);

	const auto width = lerp(mWidth[0], mWidth[1], u);

	// now, evaluate the point on the curve using w. the point is in ray space
	// dp_dw should be the tangent at this point
	const auto [point, dp_dw] = 
		evaluate_bezier_curve(control_points, clamp(w, static_cast<real>(0), static_cast<real>(1)));

	const auto distance2 = point.x * point.x + point.y * point.y;

	if (distance2 > width * width * 0.25f || point.z < 0 || point.z > ray.length) 
		return std::nullopt;

	const auto distance = sqrt(distance2);
	const auto edge_function = point.x * dp_dw.y - point.y * dp_dw.x;
	const auto v = (edge_function > 0) ? static_cast<real>(0.5) + distance / width : static_cast<real>(0.5) - distance / width;

	// evaluate the dp_du and local_point
	const auto [local_point, dp_du] = evaluate_bezier_curve(mControlPoints, u);

	// because the curve is the cylinder mode, the normal is not always same
	// transform the dp_du from local space to ray space, and build dp_dv plane
	const auto dp_du_plane = transform_vector(ray_to_local.inverse(), dp_du);
	const auto dp_dv_plane = normalize(vector3(-dp_du_plane.y, dp_du_plane.x, 0)) * width;

	// now, we can use v to find the angle the dp_dv plane should rotate
	const auto theta = lerp(static_cast<real>(-90), static_cast<real>(90), v);
	const auto rotate = shared::rotate(-theta, dp_du_plane);

	const auto dp_dv = transform_vector(ray_to_local, transform_vector(rotate, dp_dv_plane));
	const auto normal =
		reverse_orientation() ? -normalize(math::cross(dp_du, dp_dv)) : normalize(math::cross(dp_du, dp_dv));

	// update the length of ray, it is the length of vector((0, 0) - point)
	ray.length = length(point);

	// use local_point as the point of interaction
	return surface_interaction(
		nullptr,
		dp_du, dp_dv, normal, local_point, -ray.direction, vector2(u, v));
}
