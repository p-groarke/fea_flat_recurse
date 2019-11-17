#include "objs.hpp"
#include "global.hpp"
#include "iterators.hpp"

#include <fea_flat_recurse/fea_flat_recurse.hpp>
#include <gtest/gtest.h>

namespace fea {
template <>
inline std::pair<small_obj*, small_obj*> children_range(small_obj* parent) {
	if (parent->children.empty()) {
		return { nullptr, nullptr };
	}

	small_obj* beg = &parent->children.front();
	return { beg, beg + parent->children.size() };
}

template <>
std::pair<input_it<small_obj>, input_it<small_obj>> children_range(
		input_it<small_obj> root) {
	if (root->children.empty()) {
		return { nullptr, nullptr };
	}


	input_it<small_obj> beg = { &root->children.front() };
	input_it<small_obj> end
			= { &root->children.front() + root->children.size() };
	return { beg, end };
}
template <>
std::pair<bidir_it<small_obj>, bidir_it<small_obj>> children_range(
		bidir_it<small_obj> root) {
	if (root->children.empty()) {
		return { nullptr, nullptr };
	}


	bidir_it<small_obj> beg = { &root->children.front() };
	bidir_it<small_obj> end
			= { &root->children.front() + root->children.size() };
	return { beg, end };
}
} // namespace fea

size_t disable_counter = 0;

small_obj::small_obj(small_obj* _parent)
		: parent(_parent) {
}

void small_obj::create_graph(const size_t max_depth, const size_t num_children,
		const size_t depth /*= 0*/) {
	if (depth == max_depth - 1)
		return;

	++disable_counter;
	disabled = disable_counter % 6 == 0;

	children.resize(num_children, { this });

	for (size_t i = 0; i < num_children; ++i) {
		// children.push_back({});
		children[i].create_graph(max_depth, num_children, depth + 1);
	}
}

bool small_obj::operator==(const small_obj& other) const {
	return this == &other;
}

namespace {
TEST(flat_recurse, small_obj_deeper) {
	{
		small_obj root{ nullptr };
		root.create_graph(7, 7);

		SCOPED_TRACE("small_obj test breadth");
		test_breadth(&root);

		SCOPED_TRACE("small_obj test depth");
		test_depth(&root);

		// cull disabled
		{
			auto cull_pred = [](small_obj* node) { return node->disabled; };
			auto parent_cull_pred = [=](small_obj* node) {
				if (node->parent == nullptr) {
					return cull_pred(node);
				}
				return cull_pred(node->parent);
			};

			root.disabled = false;
			SCOPED_TRACE("small_obj test cull disabled");
			test_culling(&root, cull_pred, parent_cull_pred);
		}

		// cull enabled
		{
			auto cull_pred
					= [](small_obj* node) { return node->disabled == false; };
			auto parent_cull_pred = [=](small_obj* node) {
				if (node->parent == nullptr) {
					return cull_pred(node);
				}
				return cull_pred(node->parent);
			};

			root.disabled = true;
			SCOPED_TRACE("small_obj test cull enabled");
			test_culling(&root, cull_pred, parent_cull_pred);
		}
	}
}

TEST(flat_recurse, small_obj_wider) {
	// basic test
	{
		small_obj root{ nullptr };
		root.create_graph(2, 50);

		SCOPED_TRACE("small_obj test breadth");
		test_breadth(&root);

		SCOPED_TRACE("small_obj test depth");
		test_depth(&root);

		// cull disabled
		{
			auto cull_pred = [](small_obj* node) { return node->disabled; };
			auto parent_cull_pred = [=](small_obj* node) {
				if (node->parent == nullptr) {
					return cull_pred(node);
				}
				return cull_pred(node->parent);
			};

			root.disabled = false;
			SCOPED_TRACE("small_obj test cull disabled");
			test_culling(&root, cull_pred, parent_cull_pred);
		}

		// cull enabled
		{
			auto cull_pred
					= [](small_obj* node) { return node->disabled == false; };
			auto parent_cull_pred = [=](small_obj* node) {
				if (node->parent == nullptr) {
					return cull_pred(node);
				}
				return cull_pred(node->parent);
			};

			root.disabled = true;
			SCOPED_TRACE("small_obj test cull enabled");
			test_culling(&root, cull_pred, parent_cull_pred);
		}
	}
}

TEST(flat_recurse, small_obj_input_it) {
	// basic test
	{
		small_obj root{ nullptr };
		root.create_graph(6, 10);

		input_it<small_obj> root_it{ &root };

		SCOPED_TRACE("small_obj input_it test breadth");
		test_breadth(root_it);

		SCOPED_TRACE("small_obj input_it test depth");
		test_depth(root_it);

		// cull disabled
		{
			auto cull_pred = [](auto node) { return node->disabled; };
			auto parent_cull_pred = [=](auto node) {
				if (node->parent == nullptr) {
					return cull_pred(node);
				}
				return cull_pred(node->parent);
			};

			root.disabled = false;
			SCOPED_TRACE("small_obj test cull disabled");
			test_culling(root_it, cull_pred, parent_cull_pred);
		}

		// cull enabled
		{
			auto cull_pred = [](auto node) { return node->disabled == false; };
			auto parent_cull_pred = [=](auto node) {
				if (node->parent == nullptr) {
					return cull_pred(node);
				}
				return cull_pred(node->parent);
			};

			root.disabled = true;
			SCOPED_TRACE("small_obj test cull enabled");
			test_culling(root_it, cull_pred, parent_cull_pred);
		}
	}
}

TEST(flat_recurse, small_obj_bidir_it) {
	// basic test
	{
		std::vector<small_obj>::iterator vec;
		small_obj root{ nullptr };
		root.create_graph(6, 10);

		bidir_it<small_obj> root_it{ &root };

		SCOPED_TRACE("small_obj bidir_it test breadth");
		test_breadth(root_it);

		SCOPED_TRACE("small_obj bidir_it test depth");
		test_depth(root_it);

		// cull disabled
		{
			auto cull_pred = [](auto node) { return node->disabled; };
			auto parent_cull_pred = [=](auto node) {
				if (node->parent == nullptr) {
					return cull_pred(node);
				}
				return cull_pred(node->parent);
			};

			root.disabled = false;
			SCOPED_TRACE("small_obj test cull disabled");
			test_culling(root_it, cull_pred, parent_cull_pred);
		}

		// cull enabled
		{
			auto cull_pred = [](auto node) { return node->disabled == false; };
			auto parent_cull_pred = [=](auto node) {
				if (node->parent == nullptr) {
					return cull_pred(node);
				}
				return cull_pred(node->parent);
			};

			root.disabled = true;
			SCOPED_TRACE("small_obj test cull enabled");
			test_culling(root_it, cull_pred, parent_cull_pred);
		}
	}
}
} // namespace
