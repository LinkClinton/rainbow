#include "bounding_volume_hierarchy.hpp"

bool rainbow::accelerators::bounding_volume_hierarchy_node::is_leaf() const noexcept
{
	/*
	 * begin and end indicate some entities range [begin, end) that in this node
	 * so end should greater than begin in leaf node
	 */
	return end > begin;
}
