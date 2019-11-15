#include "node_uptr.hpp"
#include "global.hpp"

#include <fea_flat_recurse/fea_flat_recurse.hpp>
#include <gtest/gtest.h>

size_t node_uptr::_id_counter = 0;

namespace fea {
template <>
std::pair<std::unique_ptr<node_uptr>*, std::unique_ptr<node_uptr>*>
children_range<std::unique_ptr<node_uptr>*>(
		std::unique_ptr<node_uptr>* parent) {

	if (parent->get()->children().empty()) {
		return { nullptr, nullptr };
	}

	std::unique_ptr<node_uptr>* beg = &parent->get()->children().front();
	return { beg, beg + parent->get()->children().size() };
}
template <>
std::pair<const std::unique_ptr<node_uptr>*, const std::unique_ptr<node_uptr>*>
children_range<const std::unique_ptr<node_uptr>*>(
		const std::unique_ptr<node_uptr>* parent) {

	if (parent->get()->children().empty()) {
		return { nullptr, nullptr };
	}

	const std::unique_ptr<node_uptr>* beg = &parent->get()->children().front();
	return { beg, beg + parent->get()->children().size() };
}
} // namespace fea

namespace {


TEST(flat_recurse, breadth_basics) {
	{
		std::unique_ptr<node_uptr> root = std::make_unique<node_uptr>(nullptr);
		root->create_graph(9, 3);

		SCOPED_TRACE("breadth_basics not random");
		test_breadth(&root);

		// predicate
		auto vec = fea::gather_breadth_graph(
				&root, [](auto node) { return node->get()->disabled(); });
		for (auto it : vec) {
			EXPECT_FALSE(it->get()->disabled());
		}

		auto split_vec = fea::gather_staged_breadth_graph(
				&root, [](auto node) { return !node->get()->disabled(); });
		for (auto& v : split_vec) {
			for (auto it : v) {
				EXPECT_TRUE(it->get()->disabled());
			}
		}
	}

	// random children size
	{
		std::unique_ptr<node_uptr> root = std::make_unique<node_uptr>(nullptr);
		root->create_graph(9, 10, true);

		SCOPED_TRACE("breadth_basics random");
		test_breadth(&root);

		// predicate
		auto vec = fea::gather_breadth_graph(
				&root, [](auto node) { return node->get()->disabled(); });
		for (auto it : vec) {
			EXPECT_FALSE(it->get()->disabled());
		}

		auto split_vec = fea::gather_staged_breadth_graph(
				&root, [](auto node) { return !node->get()->disabled(); });
		for (auto& v : split_vec) {
			for (auto it : v) {
				EXPECT_TRUE(it->get()->disabled());
			}
		}
	}
}

TEST(flat_recurse, depth_basics) {
	// basic test
	{
		std::unique_ptr<node_uptr> root = std::make_unique<node_uptr>(nullptr);
		root->create_graph(2, 3);

		SCOPED_TRACE("depth_basics not random");
		test_depth(&root);

		// predicate
		std::vector<std::unique_ptr<node_uptr>*> vec;
		fea::gather_depth_graph(
				&root, &vec, [](auto node) { return node->get()->disabled(); });
		for (auto it : vec) {
			EXPECT_FALSE(it->get()->disabled());
		}

		auto vec2 = fea::gather_depth_graph_flat(
				&root, [](auto node) { return !node->get()->disabled(); });
		for (auto it : vec2) {
			EXPECT_TRUE(it->get()->disabled());
		}
	}

	// random test
	{
		std::unique_ptr<node_uptr> root = std::make_unique<node_uptr>(nullptr);
		root->create_graph(10, 3, true);

		SCOPED_TRACE("depth_basics random");
		test_depth(&root);

		// predicate
		std::vector<std::unique_ptr<node_uptr>*> vec;
		fea::gather_depth_graph(
				&root, &vec, [](auto node) { return node->get()->disabled(); });
		for (auto it : vec) {
			EXPECT_FALSE(it->get()->disabled());
		}

		auto vec2 = fea::gather_depth_graph_flat(
				&root, [](auto node) { return !node->get()->disabled(); });
		for (auto it : vec2) {
			EXPECT_TRUE(it->get()->disabled());
		}
	}
}

} // namespace
