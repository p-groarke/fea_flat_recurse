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

		std::vector<std::unique_ptr<node_uptr>*> vec
				= fea::breadth_hierarchy(&root);
		std::vector<std::unique_ptr<node_uptr>*> ref_vec;
		fea::recurse_depth_hierarchy(&root, &ref_vec);

		EXPECT_EQ(vec.size(), ref_vec.size());
	}

	// random children size
	{
		std::unique_ptr<node_uptr> root = std::make_unique<node_uptr>(nullptr);
		root->create_graph(9, 3, true);

		SCOPED_TRACE("breadth_basics random");
		test_breadth(&root);

		std::vector<std::unique_ptr<node_uptr>*> vec
				= fea::breadth_hierarchy(&root);
		std::vector<std::unique_ptr<node_uptr>*> ref_vec;
		fea::recurse_depth_hierarchy(&root, &ref_vec);

		EXPECT_EQ(vec.size(), ref_vec.size());
	}
}

TEST(flat_recurse, depth_basics) {
	// basic test
	{
		std::unique_ptr<node_uptr> root = std::make_unique<node_uptr>(nullptr);
		root->create_graph(2, 3);

		SCOPED_TRACE("depth_basics not random");
		test_depth(&root);
	}

	// random test
	{
		std::unique_ptr<node_uptr> root = std::make_unique<node_uptr>(nullptr);
		root->create_graph(10, 3, true);

		SCOPED_TRACE("depth_basics random");
		test_depth(&root);
	}
}

} // namespace
