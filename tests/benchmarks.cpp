#include "global.hpp"

#include <array>
#include <bench_util/bench_util.h>
#include <chrono>
#include <fea_flat_recurse/fea_flat_recurse.hpp>
#include <gtest/gtest.h>
#include <random>
#include <thread>
#include <vector>

#if defined(NDEBUG)

namespace {
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
} // namespace

namespace fea {

template <>
inline std::pair<small_obj*, small_obj*> children_range(small_obj* parent) {
	if (parent->children.empty()) {
		return { nullptr, nullptr };
	}

	small_obj* beg = &parent->children.front();
	return { beg, beg + parent->children.size() };
}
} // namespace fea

namespace {
size_t node_count(size_t depth, size_t width) {
	size_t ret = 0;
	for (size_t i = 0; i < depth; ++i) {
		ret += size_t(std::pow(width, i));
	}
	return ret;
}

template <class T>
void reserve_split_vec(
		size_t depth, size_t width, std::vector<std::vector<T>>* vec) {
	vec->resize(depth);
	for (size_t i = 0; i < depth; ++i) {
		vec->reserve(size_t(std::pow(width, i)));
	}
}

TEST(flat_recurse, small_obj) {

	{
		size_t depth = 25;
		size_t width = 2;
		size_t num_nodes = node_count(depth, width);
		small_obj root;
		root.create_graph(depth, width);

		// Easier profiling
		std::this_thread::sleep_for(std::chrono::seconds(1));

		bench::suite suite;
		std::string title = "Small Objects - " + std::to_string(depth)
				+ " deep, " + std::to_string(width)
				+ " wide, total nodes : " + std::to_string(num_nodes);

		std::vector<small_obj*> out;
		std::vector<std::vector<small_obj*>> out_split;

		suite.title(title.c_str());
		suite.benchmark(
				"recursion (depth)",
				[&]() { fea::recurse_depth_hierarchy(&root, &out); }, 5,
				[&]() {
					out = {};
					out.shrink_to_fit();
				});
		std::this_thread::sleep_for(std::chrono::seconds(1));
		suite.benchmark(
				"flat (depth)",
				[&]() { out = fea::flat_depth_hierarchy(&root); }, 5,
				[&]() {
					out = {};
					out.shrink_to_fit();
				});
		std::this_thread::sleep_for(std::chrono::seconds(1));
		suite.benchmark(
				"flat (breadth)",
				[&]() { out = fea::breadth_hierarchy(&root); }, 5,
				[&]() {
					out = {};
					out.shrink_to_fit();
				});
		std::this_thread::sleep_for(std::chrono::seconds(1));
		suite.benchmark(
				"flat (split breadth)",
				[&]() { out_split = fea::split_breadth_hierarchy(&root); }, 5,
				[&]() {
					out_split = {};
					out_split.shrink_to_fit();
				});
		std::this_thread::sleep_for(std::chrono::seconds(1));
		suite.print();
		suite.clear();
	}

	{
		size_t depth = 5;
		size_t width = 75;
		size_t num_nodes = node_count(depth, width);
		small_obj root;
		root.create_graph(depth, width);

		// Easier profiling
		std::this_thread::sleep_for(std::chrono::seconds(1));

		bench::suite suite;
		std::string title = "Small Objects - " + std::to_string(depth)
				+ " deep, " + std::to_string(width)
				+ " wide, total nodes : " + std::to_string(num_nodes);

		std::vector<small_obj*> out;
		std::vector<std::vector<small_obj*>> out_split;

		suite.title(title.c_str());
		suite.benchmark(
				"recursion (depth)",
				[&]() { fea::recurse_depth_hierarchy(&root, &out); }, 5,
				[&]() {
					out = {};
					out.shrink_to_fit();
				});
		std::this_thread::sleep_for(std::chrono::seconds(1));
		suite.benchmark(
				"flat (depth)",
				[&]() { out = fea::flat_depth_hierarchy(&root); }, 5,
				[&]() {
					out = {};
					out.shrink_to_fit();
				});
		std::this_thread::sleep_for(std::chrono::seconds(1));
		suite.benchmark(
				"flat (breadth)",
				[&]() { out = fea::breadth_hierarchy(&root); }, 5,
				[&]() {
					out = {};
					out.shrink_to_fit();
				});
		std::this_thread::sleep_for(std::chrono::seconds(1));
		suite.benchmark(
				"flat (split breadth)",
				[&]() { out_split = fea::split_breadth_hierarchy(&root); }, 5,
				[&]() {
					out_split = {};
					out_split.shrink_to_fit();
				});
		std::this_thread::sleep_for(std::chrono::seconds(1));
		suite.print();
		suite.clear();
	}
}
} // namespace

#endif // NDEBUG