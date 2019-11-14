#pragma once
#include <functional>
#include <utility>
#include <vector>

namespace fea {
// Specialize this, or provide begin() and end() on your node to get children
// iterators.
template <class Iter>
inline std::pair<Iter, Iter> children_range(Iter parent) {
	return { parent->begin(), parent->end() };
}

namespace detail {
template <class Func>
struct scope_guard {
	scope_guard(Func func)
			: _func(func) {
	}
	~scope_guard() {
		_func();
	}

private:
	Func _func;
};

template <class Func>
decltype(auto) make_guard(Func func) {
	return scope_guard<Func>(func);
}
} // namespace detail

// Traditional depth first recursion.
// Starts at the provided node.
// Fills out with depth first ordered iterators.
// CullPredicate is a predicate function which accepts an iterator, and returns
// true if the provided node and its sub-tree should be culled.
template <class Iter, class CullPredicate>
inline void recurse_depth_hierarchy(
		Iter node, std::vector<Iter>* out, CullPredicate cull_pred) {
	if (cull_pred(node)) {
		return;
	}

	out->push_back(node);

	using fea::children_range;
	std::pair<Iter, Iter> range = children_range(node);

	for (auto it = range.first; it != range.second; ++it) {
		recurse_depth_hierarchy(it, out, cull_pred);
	}
}

// Traditional depth first recursion.
// Starts at the provided node.
// Fills out with depth first ordered iterators.
template <class Iter>
inline void recurse_depth_hierarchy(Iter node, std::vector<Iter>* out) {
	recurse_depth_hierarchy(node, out, [](Iter) { return false; });
}


template <class Iter, class CullPredicate>
inline std::vector<Iter> flat_depth_hierarchy(
		Iter root, CullPredicate cull_pred) {
	std::vector<Iter> ret;
	std::vector<Iter> waiting;

	if (cull_pred(root)) {
		return ret;
	}

	ret.push_back(root);

	for (size_t i = 0; i < ret.size(); ++i) {
		using fea::children_range;
		std::pair<Iter, Iter> range = children_range(ret[i]);

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

		std::reverse_iterator<Iter> rbegin{ range.second };
		std::reverse_iterator<Iter> rend{ ++range.first };

		for (auto it = rbegin; it != rend; ++it) {
			Iter real_it = it.base();
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

template <class Iter>
inline std::vector<Iter> flat_depth_hierarchy(Iter root) {
	return flat_depth_hierarchy(root, [](Iter) { return false; });
}


template <class Iter, class CullPredicate>
inline std::vector<Iter> breadth_hierarchy(Iter root, CullPredicate cull_pred) {
	std::vector<Iter> ret;
	if (cull_pred(root)) {
		return ret;
	}

	ret.push_back(root);

	for (size_t i = 0; i < ret.size(); ++i) {
		using fea::children_range;
		std::pair<Iter, Iter> range = children_range(ret[i]);

		for (Iter it = range.first; it != range.second; ++it) {
			if (cull_pred(it)) {
				continue;
			}

			ret.push_back(it);
		}
	}

	return ret;
}

template <class Iter>
inline std::vector<Iter> breadth_hierarchy(Iter root) {
	return breadth_hierarchy(root, [](Iter) { return false; });

	// std::vector<Iter> ret;
	// ret.push_back(root);

	// for (size_t i = 0; i < ret.size(); ++i) {
	//	using fea::children_range;
	//	std::pair<Iter, Iter> range = children_range(ret[i]);

	//	for (Iter it = range.first; it != range.second; ++it) {
	//		ret.push_back(it);
	//	}
	//}

	// return ret;
}

template <class Iter>
inline std::vector<std::vector<Iter>> split_breadth_hierarchy(Iter root) {
	std::vector<std::vector<Iter>> ret;
	ret.push_back({ root });

	for (size_t i = 0; i < ret.size(); ++i) {
		for (size_t j = 0; j < ret[i].size(); ++j) {
			using fea::children_range;
			std::pair<Iter, Iter> range = children_range(ret[i][j]);

			if (ret.size() == i + 1) {
				ret.push_back({});
			}

			for (Iter it = range.first; it != range.second; ++it) {
				ret[i + 1].push_back(it);
			}
		}
	}

	return ret;
}
} // namespace fea
