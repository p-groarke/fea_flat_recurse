#pragma once
#include <limits>
#include <memory>
#include <random>

struct node_uptr {
	node_uptr(node_uptr* parent)
			: _id(_id_counter++)
			, _parent(parent) {
		// data.fill(0);
	}

	void create_graph(const size_t max_depth, const size_t num_children,
			bool rand_num_children = false, const size_t depth = 0) {

		if (depth == max_depth - 1)
			return;

		size_t count = num_children;
		if (rand_num_children) {
			std::random_device rd;
			std::mt19937_64 gen(rd());
			std::uniform_int_distribution<size_t> dis(1, num_children);
			count = dis(gen);
		}

		for (size_t i = 0; i < count; ++i) {
			_children.push_back(std::make_unique<node_uptr>(this));

			_children.back()->create_graph(
					max_depth, num_children, rand_num_children, depth + 1);
		}
	}

	const std::vector<std::unique_ptr<node_uptr>>& children() const {
		return _children;
	}
	std::vector<std::unique_ptr<node_uptr>>& children() {
		return _children;
	}

	bool operator==(const node_uptr& other) const {
		return _id == other._id;
	}

private:
	size_t _id = std::numeric_limits<size_t>::max();
	node_uptr* _parent = nullptr; // observer_ptr
	std::vector<std::unique_ptr<node_uptr>> _children;

	static size_t _id_counter;
};
