#pragma once
#include <vector>

struct small_obj {
	void create_graph(const size_t max_depth, const size_t num_children,
			const size_t depth = 0) {

		if (depth == max_depth - 1)
			return;

		children.resize(num_children);

		for (size_t i = 0; i < num_children; ++i) {
			// children.push_back({});
			children[i].create_graph(max_depth, num_children, depth + 1);
		}
	}

	std::vector<small_obj> children;
	// std::array<char, 512> data{};
};
