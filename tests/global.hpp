#pragma once
#include <fea_flat_recurse/fea_flat_recurse.hpp>
#include <gtest/gtest.h>
#include <iterator>
#include <memory>

template <class InputIt>
inline void test_breadth(InputIt root) {
	const InputIt croot = root;

	std::vector<InputIt> ref_vec;
	fea::gather_depth_graph_recursive(root, &ref_vec);

	// linear breadth non-const
	{
		std::vector<InputIt> breadth_graph = fea::gather_breadth_graph(root);
		EXPECT_EQ(breadth_graph.size(), ref_vec.size());

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
		auto breadth_graph = fea::gather_breadth_graph(croot);
		EXPECT_EQ(breadth_graph.size(), ref_vec.size());

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
				= fea::gather_staged_breadth_graph(root);

		size_t staged_size = 0;
		for (auto& v : split_breadth_graph) {
			staged_size += v.size();
		}
		EXPECT_EQ(staged_size, ref_vec.size());


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
		auto split_breadth_graph = fea::gather_staged_breadth_graph(croot);

		size_t staged_size = 0;
		for (auto& v : split_breadth_graph) {
			staged_size += v.size();
		}
		EXPECT_EQ(staged_size, ref_vec.size());

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

namespace detail {
template <class InputIt>
inline void test_depth_flat(InputIt root, std::bidirectional_iterator_tag) {
	const InputIt croot = root;

	// non-const
	{
		std::vector<InputIt> depth_graph = fea::gather_depth_graph_flat(root);

		std::vector<InputIt> recursed_depth_graph;
		fea::gather_depth_graph_recursive(root, &recursed_depth_graph);

		EXPECT_EQ(depth_graph.size(), recursed_depth_graph.size());
		EXPECT_EQ(depth_graph, recursed_depth_graph);
	}

	// const
	{
		auto depth_graph = fea::gather_depth_graph_flat(croot);

		decltype(depth_graph) recursed_depth_graph;
		fea::gather_depth_graph_recursive(croot, &recursed_depth_graph);

		EXPECT_EQ(depth_graph.size(), recursed_depth_graph.size());
		EXPECT_EQ(depth_graph, recursed_depth_graph);
	}
}

template <class InputIt>
inline void test_depth_flat(InputIt root, std::input_iterator_tag) {
	const InputIt croot = root;

	// non-const
	{
		std::vector<InputIt> breadth_graph = fea::gather_breadth_graph(root);

		std::vector<InputIt> recursed_depth_graph;
		fea::gather_depth_graph_recursive(root, &recursed_depth_graph);

		EXPECT_EQ(breadth_graph.size(), recursed_depth_graph.size());
		// EXPECT_EQ(depth_graph, recursed_depth_graph);
	}

	// const
	{
		auto breadth_graph = fea::gather_breadth_graph(croot);

		decltype(breadth_graph) recursed_depth_graph;
		fea::gather_depth_graph_recursive(croot, &recursed_depth_graph);

		EXPECT_EQ(breadth_graph.size(), recursed_depth_graph.size());
		// EXPECT_EQ(breadth_graph, recursed_depth_graph);
	}
}

} // namespace detail

template <class InputIt>
inline void test_depth(InputIt root) {
	detail::test_depth_flat(
			root, std::iterator_traits<InputIt>::iterator_category());
}

namespace detail {

template <class InputIt, class CullPred, class ParentCullPred>
inline void test_culling_flat_depth(InputIt root, CullPred cull_pred,
		ParentCullPred p_cull_pred, std::bidirectional_iterator_tag) {
	std::vector<InputIt> depth_graph
			= fea::gather_depth_graph_flat(root, cull_pred);
	for (auto it : depth_graph) {
		bool p = cull_pred(it);
		bool pp = p_cull_pred(it);
		EXPECT_FALSE(p);
		EXPECT_FALSE(pp);
	}
}
template <class InputIt, class CullPred, class ParentCullPred>
inline void test_culling_flat_depth(
		InputIt, CullPred, ParentCullPred, std::input_iterator_tag) {
}
} // namespace detail

template <class InputIt, class CullPred, class ParentCullPred>
inline void test_culling(
		InputIt root, CullPred cull_pred, ParentCullPred p_cull_pred) {
	const InputIt croot = root;

	// non-const
	{
		std::vector<InputIt> breadth_graph
				= fea::gather_breadth_graph(root, cull_pred);
		for (auto it : breadth_graph) {
			bool p = cull_pred(it);
			bool pp = p_cull_pred(it);
			EXPECT_FALSE(p);
			EXPECT_FALSE(pp);
		}

		std::vector<std::vector<InputIt>> staged_breadth_graph
				= fea::gather_staged_breadth_graph(root, cull_pred);
		for (auto& v : staged_breadth_graph) {
			for (auto it : v) {
				bool p = cull_pred(it);
				bool pp = p_cull_pred(it);
				EXPECT_FALSE(p);
				EXPECT_FALSE(pp);
			}
		}

		std::vector<InputIt> recursed_depth_graph;
		fea::gather_depth_graph_recursive(
				root, &recursed_depth_graph, cull_pred);
		for (auto it : recursed_depth_graph) {
			bool p = cull_pred(it);
			bool pp = p_cull_pred(it);
			EXPECT_FALSE(p);
			EXPECT_FALSE(pp);
		}

		detail::test_culling_flat_depth(root, cull_pred, p_cull_pred,
				std::iterator_traits<InputIt>::iterator_category());
	}

	// non-const
	{
		auto breadth_graph = fea::gather_breadth_graph(croot, cull_pred);
		for (auto it : breadth_graph) {
			bool p = cull_pred(it);
			bool pp = p_cull_pred(it);
			EXPECT_FALSE(p);
			EXPECT_FALSE(pp);
		}

		auto staged_breadth_graph
				= fea::gather_staged_breadth_graph(croot, cull_pred);
		for (auto& v : staged_breadth_graph) {
			for (auto it : v) {
				bool p = cull_pred(it);
				bool pp = p_cull_pred(it);
				EXPECT_FALSE(p);
				EXPECT_FALSE(pp);
			}
		}

		decltype(breadth_graph) recursed_depth_graph;
		fea::gather_depth_graph_recursive(
				croot, &recursed_depth_graph, cull_pred);
		for (auto it : recursed_depth_graph) {
			bool p = cull_pred(it);
			bool pp = p_cull_pred(it);
			EXPECT_FALSE(p);
			EXPECT_FALSE(pp);
		}

		detail::test_culling_flat_depth(croot, cull_pred, p_cull_pred,
				std::iterator_traits<InputIt>::iterator_category());
	}
}