#ifndef AVLTREE_H
#define AVLTREE_H

namespace DataStructures
{
	template <class T>
	struct Node
	{
		T value; // must be comparable
		Node<T>* parent; // pointer to parent, null if root node
		Node<T>* left; // pointer to left child, null if leaf node
		Node<T>* right; // pointer to right child, null if leaf node

		std::size_t height; // height of the node in the tree, 0 if leaf node
		int balanceFactor; // balance factor of current node, will be in the range -2 - 2

		// constructor
		// MUST be passed a value, parent, left, and right pointers default to nullptr if not passed
		// height and balanceFactor are set to zero upon every creation
		Node(T i_value, Node<T>* i_parent=nullptr, Node<T>* i_left=nullptr, Node<T>* i_right=nullptr) :
			value{i_value}, parent{i_parent}, left{i_left}, right{i_right}, height{0}, balanceFactor{0}
		{}
	};
}

#endif
