#pragma once
#include <random>
#include <vector>

struct small_obj {
	small_obj(small_obj* _parent);

	void create_graph(const size_t max_depth, const size_t num_children,
			const size_t depth = 0);

	bool operator==(const small_obj& other) const;


	std::vector<small_obj> children;
	small_obj* parent = nullptr;
	bool disabled = false;
};
