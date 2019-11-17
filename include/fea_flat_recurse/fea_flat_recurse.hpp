#pragma once
/*
BSD 3-Clause License

Copyright (c) 2019, Philippe Groarke
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
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


// for_each

// Traditional depth-first recursion.
// Starts at the provided node.
// Executes func on each node.
// CullPredicate accepts an iterator and returns true if the node and its
// sub-tree should be culled.
template <class InputIt, class Func, class CullPredicate>
inline void for_each_recursive_depth(
		InputIt root, Func func, CullPredicate cull_pred) {
	if (cull_pred(root)) {
		return;
	}

	func(root);

	using fea::children_range;
	std::pair<InputIt, InputIt> range = children_range(root);

	for (auto it = range.first; it != range.second; ++it) {
		for_each_recursive_depth(it, func, cull_pred);
	}
}

// Traditional depth-first recursion.
// Starts at the provided node.
// Executes func on each node.
template <class InputIt, class Func, class CullPredicate>
inline void for_each_recursive_depth(InputIt root, Func func) {
	return for_each_recursive_depth(root, func, [](InputIt) { return false; });
}


// Flat depth-first iteration.
// Starts at the provided node.
// Executes func on each node.
// CullPredicate accepts an iterator and returns true if the node and its
// sub-tree should be culled.
template <class BidirIt, class Func, class CullPredicate>
inline void for_each_flat_depth(
		BidirIt root, Func func, CullPredicate cull_pred) {
	if (cull_pred(root)) {
		return;
	}

	BidirIt current_node = root;
	func(root);

	std::vector<BidirIt> waiting;

	while (true) {
		using fea::children_range;
		std::pair<BidirIt, BidirIt> range = children_range(current_node);

		if (range.first == range.second) {
			if (!waiting.empty()) {
				current_node = waiting.back();
				waiting.pop_back();
				func(current_node);
			} else {
				break;
			}
			continue;
		}

		bool cullit = cull_pred(range.first);
		if (!cullit) {
			current_node = range.first;
			func(current_node);
		}

		std::reverse_iterator<BidirIt> rbegin{ range.second };
		std::reverse_iterator<BidirIt> rend{ ++range.first };

		for (auto it = rbegin; it != rend; ++it) {
			BidirIt real_it = it.base();
			if (cull_pred(--real_it)) {
				continue;
			}
			waiting.push_back(real_it);
		}

		if (cullit) {
			if (!waiting.empty()) {
				current_node = waiting.back();
				waiting.pop_back();
				func(current_node);
			} else {
				break;
			}
		}
	}
}

// Flat depth-first iteration.
// Starts at the provided node.
// Executes func on each node.
template <class BidirIt, class Func>
inline void for_each_flat_depth(BidirIt root, Func func) {
	return for_each_flat_depth(root, func, [](BidirIt) { return false; });
}


// gather

// Gathers nodes using a traditional depth-first recursion.
// Starts at the provided node.
// Fills out with depth first ordered iterators.
// CullPredicate is a predicate function which accepts an iterator, and returns
// true if the provided node and its sub-tree should be culled.
template <class InputIt, class CullPredicate>
inline void gather_depth_graph_recursive(
		InputIt root, std::vector<InputIt>* out, CullPredicate cull_pred) {
	return for_each_recursive_depth(
			root, [&](InputIt node) { out->push_back(node); }, cull_pred);
}

// Gathers nodes using a traditional depth-first recursion.
// Starts at the provided node.
// Fills out with depth first ordered iterators.
template <class InputIt>
inline void gather_depth_graph_recursive(
		InputIt root, std::vector<InputIt>* out) {
	return gather_depth_graph_recursive(
			root, out, [](InputIt) { return false; });
}


// Gathers a depth-first flat vector without recursing.
// Starts at the provided node.
// Returns depth first ordered iterators.
// CullPredicate is a predicate function which accepts an iterator, and returns
// true if the provided node and its sub-tree should be culled.
template <class BidirIt, class CullPredicate>
inline std::vector<BidirIt> gather_depth_graph_flat(
		BidirIt root, CullPredicate cull_pred) {
	std::vector<BidirIt> ret;
	for_each_flat_depth(
			root, [&](BidirIt node) { ret.push_back(node); }, cull_pred);
	return ret;
}

// Gathers a depth-first flat vector without recursing.
// Starts at the provided node.
// Returns depth first ordered iterators.
template <class BidirIt>
inline std::vector<BidirIt> gather_depth_graph_flat(BidirIt root) {
	return gather_depth_graph_flat(root, [](BidirIt) { return false; });
}


// Gathers a breadth-first flat vector without recursing.
// Starts at the provided node.
// Returns breadth first ordered iterators.
// CullPredicate is a predicate function which accepts an iterator, and returns
// true if the provided node and its sub-tree should be culled.
template <class InputIt, class CullPredicate>
inline std::vector<InputIt> gather_breadth_graph(
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

// Gathers a breadth-first flat vector without recursing.
// Starts at the provided node.
// Returns breadth first ordered iterators.
template <class InputIt>
inline std::vector<InputIt> gather_breadth_graph(InputIt root) {
	return gather_breadth_graph(root, [](InputIt) { return false; });
}


// Flat breadth-first iteration.
// Fills up a vector internally, use the gather function if you call this on the
// same graph more than once!
// Starts at the provided node.
// Executes func on each node.
// CullPredicate accepts an iterator and returns true if the node and its
// sub-tree should be culled.
template <class InputIt, class Func, class CullPredicate>
inline void for_each_breadth(InputIt root, Func func, CullPredicate cull_pred) {
	std::vector<InputIt> graph = gather_breadth_graph(root, cull_pred);
	for (InputIt it : graph) {
		func(it);
	}
}

// Flat breadth-first iteration.
// Fills up a vector internally, use the gather function if you call this on the
// same graph more than once!
// Starts at the provided node.
// Executes func on each node.
template <class InputIt, class Func>
inline void for_each_breadth(InputIt root, Func func) {
	std::vector<InputIt> graph = gather_breadth_graph(root);
	for (InputIt it : graph) {
		func(it);
	}
}


// Gathers a breadth-first vector of vector without recursing. Sub vectors are
// the breadths. Useful for multithreading.
// Starts at the provided node.
// Returns vector of breadth iterator vectors.
// CullPredicate is a predicate function which accepts an iterator, and returns
// true if the provided node and its sub-tree should be culled.
template <class InputIt, class CullPredicate>
inline std::vector<std::vector<InputIt>> gather_staged_breadth_graph(
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

// Gathers a breadth-first vector of vector without recursing. Sub vectors are
// the breadths. Useful for multithreading.
// Starts at the provided node.
// Returns vector of breadth iterator vectors.
template <class InputIt>
inline std::vector<std::vector<InputIt>> gather_staged_breadth_graph(
		InputIt root) {
	return gather_staged_breadth_graph(root, [](InputIt) { return false; });
}
} // namespace fea
