#pragma once
#include <fea_flat_recurse/fea_flat_recurse.hpp>
#include <gtest/gtest.h>
#include <memory>

template <class InputIt>
inline void test_breadth(InputIt root) {
	const InputIt croot = root;

	// linear breadth non-const
	{
		std::vector<InputIt> breadth_graph = fea::breadth_hierarchy(root);

		size_t next_breadth_start = 1;
		for (size_t i = 0; i < breadth_graph.size(); ++i) {
			auto node = breadth_graph[i];

			using fea::children_range;
			auto range = children_range(node);

			size_t j = 0;
			for (auto it = range.first; it != range.second; ++it) {
				size_t child_pos = j + next_breadth_start;

				// if ((*breadth_graph[child_pos])->id != child->id) {
				//	printf("");
				//}

				EXPECT_EQ(*breadth_graph[child_pos], *it);
				++j;
			}

			next_breadth_start += std::distance(range.first, range.second);
		}
	}

	// linear breadth const
	{
		auto breadth_graph = fea::breadth_hierarchy(croot);

		size_t next_breadth_start = 1;
		for (size_t i = 0; i < breadth_graph.size(); ++i) {
			const auto node = breadth_graph[i];

			using fea::children_range;
			auto range = children_range(node);

			size_t j = 0;
			for (auto it = range.first; it != range.second; ++it) {
				size_t child_pos = j + next_breadth_start;

				// if ((*breadth_graph[child_pos])->id != child->id) {
				//	printf("");
				//}

				EXPECT_EQ(*breadth_graph[child_pos], *it);
				++j;
			}

			next_breadth_start += std::distance(range.first, range.second);
		}
	}

	// split breadth non-const
	{
		std::vector<std::vector<InputIt>> split_breadth_graph
				= fea::split_breadth_hierarchy(root);

		for (size_t i = 0; i < split_breadth_graph.size(); ++i) {
			std::vector<InputIt>& vec = split_breadth_graph[i];

			size_t next_breadth_start = 0;

			for (size_t j = 0; j < vec.size(); ++j) {
				const auto node = split_breadth_graph[i][j];

				using fea::children_range;
				auto range = children_range(node);

				size_t k = 0;
				for (auto it = range.first; it != range.second; ++it) {
					size_t child_pos = k + next_breadth_start;

					// if ((*breadth_graph[child_pos])->id != child->id) {
					//	printf("");
					//}

					EXPECT_EQ(*split_breadth_graph[i + 1][child_pos], *it);
					++k;
				}

				next_breadth_start += std::distance(range.first, range.second);
			}
		}
	}

	// split breadth const
	{
		auto split_breadth_graph = fea::split_breadth_hierarchy(croot);

		for (size_t i = 0; i < split_breadth_graph.size(); ++i) {
			auto& vec = split_breadth_graph[i];

			size_t next_breadth_start = 0;

			for (size_t j = 0; j < vec.size(); ++j) {
				const auto node = split_breadth_graph[i][j];

				using fea::children_range;
				auto range = children_range(node);

				size_t k = 0;
				for (auto it = range.first; it != range.second; ++it) {
					size_t child_pos = k + next_breadth_start;

					// if (*split_breadth_graph[i + 1][child_pos] != *it) {
					//	printf("");
					//}

					EXPECT_EQ(*split_breadth_graph[i + 1][child_pos], *it);
					++k;
				}

				next_breadth_start += std::distance(range.first, range.second);
			}
		}
	}
}

template <class InputIt>
inline void test_depth(InputIt root) {
	const InputIt croot = root;

	// non-const
	{
		std::vector<InputIt> depth_graph = fea::flat_depth_hierarchy(root);

		std::vector<InputIt> recursed_depth_graph;
		fea::recurse_depth_hierarchy(root, &recursed_depth_graph);

		EXPECT_EQ(depth_graph.size(), recursed_depth_graph.size());
		EXPECT_EQ(depth_graph, recursed_depth_graph);
	}

	// const
	{
		auto depth_graph = fea::flat_depth_hierarchy(croot);

		decltype(depth_graph) recursed_depth_graph;
		fea::recurse_depth_hierarchy(croot, &recursed_depth_graph);

		EXPECT_EQ(depth_graph.size(), recursed_depth_graph.size());
		EXPECT_EQ(depth_graph, recursed_depth_graph);
	}
}
