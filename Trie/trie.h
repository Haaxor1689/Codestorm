#ifndef TRIE_H
#define TRIE_H

#include <memory>
#include <vector>

template<typename Value>
class Trie {
	using Seq = std::vector< bool >;
public:
	/**
	 * @brief	class representing inner node of Binary Trie
	 */
	class Node {
	public:
		/**
		 * @brief	left child Node getter
		 * @return 	raw pointer to the left children Node
		 */
		const Node* left() const noexcept {
			return _left.get();
		}

		/**
		 * @brief	right child Node getter
		 * @return 	raw pointer to the right children Node
		 */
		const Node* right() const noexcept {
			return _right.get();
		}

		/**
		 * @brief	value getter
		 * @return 	raw pointer to the saved value (nullptr if no value is present)
		 */
		const Value* value() const noexcept {
			return _value.get();
		}

	private:
		friend class Trie;
		std::unique_ptr<Value> _value;
		std::unique_ptr<Node> _left;
		std::unique_ptr<Node> _right;
	};

	/**
	 * @brief default ctor
	 */
	Trie() = default;

	/**
	 * @brief	copy ctor
	 * @param 	other		instance of Trie class
	 */
	Trie(const Trie& other) {
		_copy(_root.get(), other._root.get());
	}

	/**
	 * @brief	root getter
	 * @return	returns const reference to the root
	 * 			NOTE: root already exists
	 */
	const Node& root() const noexcept {
		return *_root;
	}

	/**
	 * @brief	inserts element to the Trie
	 * 			if element is already present in Trie insertion will not take a place
	 * @param 	seq			sequence vector of booleans
	 * @param 	val			value to be inserted
	 * @return	true if insertion succeeds, false otherwise
	 */
	bool insert(const Seq& seq, const Value& val) {
		const Node* node = _root.get();
		for (bool r : seq) {
			if (r) {
				if (!node->right())
					const_cast<Node*>(node)->_right = std::make_unique<Node>();
				node = node->right();
			} else {
				if (!node->left())
					const_cast<Node*>(node)->_left = std::make_unique<Node>();
				node = node->left();
			}
		}
		if (node->value())
			return false;
		const_cast<Node*>(node)->_value = std::make_unique<Value>(val);
		return true;
	}

	/**
	 * @brief	searches for value
	 * @param 	seq			sequence vector of booleans
	 * @return 	const raw pointer to the value, nullptr if value is not present
	 */
	const Value* search(const Seq& seq) const noexcept {
		const Node* node = _search(seq);
		return node ? node->value() : nullptr;
	}

	/**
	 * @brief	searches for value
	 * @param 	seq			sequence vector of booleans
	 * @return 	raw pointer to the value, nullptr if value is not present
	 */
	Value* search(const Seq& seq) noexcept {
		return const_cast<Value*>(const_cast<const Trie*>(this)->search(seq));
	}

	/**
	 * @brief	removes element
	 * @param 	seq 		sequence vector of booleans
	 */
	void remove(const Seq& seq) {
		Node* node = const_cast<Node*>(_search(seq));
		if (node) {
			node->_value.reset();
			_clear_leaves(_root.get());
		}
	}

	/**
	 * @brief	unites two Tries
	 * @param 	other 		other Trie
	 * @param 	zip 		function objects to zip values
	 */
	template< typename Zip >
	void uniteWith(const Trie& other, Zip zip) {
		_uniteWith(_root.get(), other._root.get(), zip);
	}

	/**
	 * @brief	intersects two Tries
	 * @param 	other		other Trie
	 * @param 	zip 		function object to zip values
	 */
	template< typename Zip >
	void intersectWith(const Trie& other, Zip zip) {
		_intersectWith(_root.get(), other._root.get(), zip);
		_clear_leaves(_root.get());
	}

private:
	std::unique_ptr<Node> _root = std::make_unique<Node>();

	/**
	 * @brief	copies Nodes from one Trie to another
	 * 			(used by copy ctor)
	 * @param 	to			Node to which is being copied
	 * @param 	from 		Node from which is being copied
	 */
	void _copy(Node* to, const Node* from) {
		if (from->value())
			to->_value = std::make_unique<Value>(*from->value());
		if (from->left()) {
			to->_left = std::make_unique<Node>();
			_copy(const_cast<Node*>(to->left()), from->left());
		}
		if (from->right()) {
			to->_right = std::make_unique<Node>();
			_copy(const_cast<Node*>(to->right()), from->right());
		}
	}

	/**
	 * @brief	searches for Node at provided Sequence
	 * @param 	seq			sequence vector of booleans
	 * @return	raw pointer to Node if Node is present, nullptr otherwise
	 */
	const Node* _search(const Seq& seq) const noexcept {
		const Node* node = _root.get();
		for (bool r : seq) {
			if (r) {
				if (!node->right())
					return nullptr;
				node = node->right();
			} else {
				if (!node->left())
					return nullptr;
				node = node->left();
			}
		}
		return node;
	}

	/**
	 * @brief	checks whether node is leaf or not
	 * 			(leaf = no value && no children)
	 * @param 	node
	 * @return 	true if node is leaf, false otherwise
	 */
	bool _is_leaf(const Node* node) const noexcept {
		return !node->value() && !node->left() && !node->right();
	}

	/**
	 * @brief	clears leaves from Node
	 * @param 	node
	 */
	void _clear_leaves(const Node* node) noexcept {
		if (node->left()) {
			_clear_leaves(node->left());
			if (_is_leaf(node->left()))
				const_cast<Node*>(node)->_left.reset();
		}

		if (node->right()) {
			_clear_leaves(node->right());
			if (_is_leaf(node->right()))
				const_cast<Node*>(node)->_right.reset();
		}
	}

	/**
	 * @brief	recursive function used in uniteWith
	 * @param 	to 			raw pointer to the Node of Trie which will be result of union
	 * @param 	with 		raw pointer to the Node of Trie with which Trie is being united
	 * @param 	zip			zipping function
	 */
	template <typename Zip>
	void _uniteWith(Node* to, const Node* with, Zip zip) {
		if (with->value()) {
			if (to->value()) {
				*to->_value = zip(*to->value(), *with->value());
			} else {
				to->_value = std::make_unique<Value>(*with->value());
			}
		}
		if (with->left()) {
			if (!to->left())
				to->_left = std::make_unique<Node>();
			_uniteWith(const_cast<Node*>(to->left()), with->left(), zip);
		}
		if (with->right()) {
			if (!to->right())
				to->_right = std::make_unique<Node>();
			_uniteWith(const_cast<Node*>(to->right()), with->right(), zip);
		}
	}

	/**
	 * @brief	recursive function used by intersectWith
	 * @param 	to 			raw pointer to the Node of Trie which will be result of intersect
	 * @param 	with 		raw pointer to the Node of Trie with which Trie is being intersected
	 * @param 	zip			zipping function
	 */
	template <typename Zip>
	void _intersectWith(Node* to, const Node* with, Zip zip) {
		if (with->value()) {
			if (to->value()) {
				*to->_value = zip(*to->value(), *with->value());
			} else {
				to->_value.reset();
			}
		}
		if (!with->left()) {
			if (to->left())
				to->_left.reset();
		} else {
			_intersectWith(const_cast<Node*>(to->left()), with->left(), zip);
		}
		if (!with->right()) {
			if (to->right())
				to->_right.reset();
		} else {
			_intersectWith(const_cast<Node*>(to->right()), with->right(), zip);
		}
	}
};

#endif
