#include "global.hpp"
#include "small_obj.hpp"

#include <array>
#include <bench_util/bench_util.h>
#include <chrono>
#include <fea_flat_recurse/fea_flat_recurse.hpp>
#include <gtest/gtest.h>
#include <random>
#include <thread>

#if defined(NDEBUG)

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
				[&]() { fea::gather_depth_graph_recursive(&root, &out); }, 5,
				[&]() {
					out = {};
					out.shrink_to_fit();
				});
		std::this_thread::sleep_for(std::chrono::seconds(1));
		suite.benchmark(
				"flat (depth)",
				[&]() { out = fea::gather_depth_graph_flat(&root); }, 5,
				[&]() {
					out = {};
					out.shrink_to_fit();
				});
		std::this_thread::sleep_for(std::chrono::seconds(1));
		suite.benchmark(
				"flat (breadth)",
				[&]() { out = fea::gather_breadth_graph(&root); }, 5,
				[&]() {
					out = {};
					out.shrink_to_fit();
				});
		std::this_thread::sleep_for(std::chrono::seconds(1));
		suite.benchmark(
				"flat (split breadth)",
				[&]() { out_split = fea::gather_staged_breadth_graph(&root); },
				5,
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
				[&]() { fea::gather_depth_graph_recursive(&root, &out); }, 5,
				[&]() {
					out = {};
					out.shrink_to_fit();
				});
		std::this_thread::sleep_for(std::chrono::seconds(1));
		suite.benchmark(
				"flat (depth)",
				[&]() { out = fea::gather_depth_graph_flat(&root); }, 5,
				[&]() {
					out = {};
					out.shrink_to_fit();
				});
		std::this_thread::sleep_for(std::chrono::seconds(1));
		suite.benchmark(
				"flat (breadth)",
				[&]() { out = fea::gather_breadth_graph(&root); }, 5,
				[&]() {
					out = {};
					out.shrink_to_fit();
				});
		std::this_thread::sleep_for(std::chrono::seconds(1));
		suite.benchmark(
				"flat (split breadth)",
				[&]() { out_split = fea::gather_staged_breadth_graph(&root); },
				5,
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