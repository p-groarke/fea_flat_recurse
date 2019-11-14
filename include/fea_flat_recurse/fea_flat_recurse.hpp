#pragma once
#include <functional>
#include <utility>
#include <vector>

namespace fea {
// Specialize this, or provide begin() and end() on your node to get children
// iterators.
template <class InputIt>
inline std::pair<InputIt, InputIt> children_range(InputIt parent) {
	return { parent->begin(), parent->end() };
}

// Traditional depth first recursion.
// Starts at the provided node.
// Fills out with depth first ordered iterators.
// CullPredicate is a predicate function which accepts an iterator, and returns
// true if the provided node and its sub-tree should be culled.
template <class InputIt, class CullPredicate>
inline void recurse_depth_hierarchy(
		InputIt node, std::vector<InputIt>* out, CullPredicate cull_pred) {
	if (cull_pred(node)) {
		return;
	}

	out->push_back(node);

	using fea::children_range;
	std::pair<InputIt, InputIt> range = children_range(node);

	for (auto it = range.first; it != range.second; ++it) {
		recurse_depth_hierarchy(it, out, cull_pred);
	}
}

// Traditional depth first recursion.
// Starts at the provided node.
// Fills out with depth first ordered iterators.
template <class InputIt>
inline void recurse_depth_hierarchy(InputIt node, std::vector<InputIt>* out) {
	recurse_depth_hierarchy(node, out, [](InputIt) { return false; });
}


template <class InputIt, class CullPredicate>
inline std::vector<InputIt> flat_depth_hierarchy(
		InputIt root, CullPredicate cull_pred) {
	std::vector<InputIt> ret;
	std::vector<InputIt> waiting;

	if (cull_pred(root)) {
		return ret;
	}

	ret.push_back(root);

	for (size_t i = 0; i < ret.size(); ++i) {
		using fea::children_range;
		std::pair<InputIt, InputIt> range = children_range(ret[i]);

		if (range.first == range.second) {
			if (!waiting.empty()) {
				ret.push_back(waiting.back());
				waiting.pop_back();
			}
			continue;
		}

		bool cullit = cull_pred(range.first);
		if (!cullit) {
			ret.push_back(range.first);
		}

		std::reverse_iterator<InputIt> rbegin{ range.second };
		std::reverse_iterator<InputIt> rend{ ++range.first };

		for (auto it = rbegin; it != rend; ++it) {
			InputIt real_it = it.base();
			waiting.push_back(--real_it);
		}

		if (cullit) {
			if (!waiting.empty()) {
				ret.push_back(waiting.back());
				waiting.pop_back();
			}
		}
	}

	return ret;
}

template <class InputIt>
inline std::vector<InputIt> flat_depth_hierarchy(InputIt root) {
	return flat_depth_hierarchy(root, [](InputIt) { return false; });
}


template <class InputIt, class CullPredicate>
inline std::vector<InputIt> breadth_hierarchy(
		InputIt root, CullPredicate cull_pred) {
	std::vector<InputIt> ret;
	if (cull_pred(root)) {
		return ret;
	}

	ret.push_back(root);

	for (size_t i = 0; i < ret.size(); ++i) {
		using fea::children_range;
		std::pair<InputIt, InputIt> range = children_range(ret[i]);

		for (InputIt it = range.first; it != range.second; ++it) {
			if (cull_pred(it)) {
				continue;
			}

			ret.push_back(it);
		}
	}

	return ret;
}

template <class InputIt>
inline std::vector<InputIt> breadth_hierarchy(InputIt root) {
	return breadth_hierarchy(root, [](InputIt) { return false; });
}


template <class InputIt, class CullPredicate>
inline std::vector<std::vector<InputIt>> split_breadth_hierarchy(
		InputIt root, CullPredicate cull_pred) {
	std::vector<std::vector<InputIt>> ret;
	if (cull_pred(root)) {
		return ret;
	}

	ret.push_back({ root });

	for (size_t i = 0; i < ret.size(); ++i) {
		for (size_t j = 0; j < ret[i].size(); ++j) {
			using fea::children_range;
			std::pair<InputIt, InputIt> range = children_range(ret[i][j]);

			if (ret.size() == i + 1) {
				ret.push_back({});
			}

			for (InputIt it = range.first; it != range.second; ++it) {
				if (cull_pred(it)) {
					continue;
				}
				ret[i + 1].push_back(it);
			}
		}
	}

	return ret;
}

template <class InputIt>
inline std::vector<std::vector<InputIt>> split_breadth_hierarchy(InputIt root) {
	return split_breadth_hierarchy(root, [](InputIt) { return false; });
}
} // namespace fea
