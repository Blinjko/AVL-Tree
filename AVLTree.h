#ifndef AVLTREE_H
#define AVLTREE_H

#include <stack>
#include <queue>
#include <stdexcept>

namespace DataStructures
{
	template <class T>
	class AVLTree
	{
		struct Node
		{
			T value;            // must be comparable
			Node* parent;       // pointer to parent, null if root node
			Node* left;         // pointer to left child, null if leaf node
			Node* right;        // pointer to right child, null if leaf node

			std::size_t height; // height of the node in the tree, 0 if leaf node
			int balanceFactor;  // balance factor of current node, will be in the range -2 - 2

			// constructor
			// MUST be passed a value, parent, left, and right pointers default to nullptr if not passed
			// height and balanceFactor are set to zero upon every creation
			Node(T i_value, Node* i_parent=nullptr, Node* i_left=nullptr, Node* i_right=nullptr) :
				value{i_value}, parent{i_parent}, left{i_left}, right{i_right}, height{0}, balanceFactor{0}
			{}
		};

		std::size_t m_size; // size of the tree, starts at 0
		Node* m_root;       // pointer to the root node, if tree is empty m_root is nullptr

		public:

		AVLTree();                                      // constructor
		AVLTree(const AVLTree<T>&) = delete;            // copy constructor disabled
		~AVLTree();                                     // destructor

		T* insert(const T&);                            // insert an element into the tree, returns a pointer to an element if it already exists, otherwise returns nullptr
		T remove(const T&);                             // remove an element from the tree, returns the value removed, if it does not exist an exception is thrown

		T* find(const T&);                              // trys to find an element given a value, if found it returns a pointer to the element, if not returns nullptr
		const T* find(const T&) const;                  // const version of find

		bool empty() const;                             // returns true if the tree is empty, false if not
		std::size_t size() const { return m_size; } 	// returns the size of the tree

		T* root() { return m_root; }                    // returns the root node pointer
		const T* root() const { return m_root; }      	// const version of root() 

		private:

		std::stack<Node*> stackNodes(const T&);         // trys to find matching node given a value, but every node that is iterated through is added to a stack
		                                                // if matching node is found, the stack is returned, the top most element being the node with the matching value
								// if no matching node is found, a stack will still be returned, but the top most element would be nullptr

		void unstackNodes(std::stack<Node*>&);          // unstacks the given stack of node pointers, updating and balancing each node as it is unstacked

		void update(Node*);                             // updates the given nodes heigh and balance factor
		void balance(Node*);                            // balances the given node

		void rightRotation(Node*);                      // does a right rotation on a given node
		void leftRotation(Node*);                       // does a left rotation on a given node

		void leafRemove(Node*);                         // removes a leaf node, assumes caller has passed a leaf node
		void oneSubtreeRemove(Node*);                   // removes a node that has one subtree, assumes caller has passed such a node
		void twoSubtreeRemove(Node*);                   // removes a node that has two subtrees, assumes caller ahs passed such a node

	};

	template <typename T>
	AVLTree<T>::AVLTree() : m_size{0}, m_root{nullptr}   // constructor start //  
	{}                                                   // constructor end //

	template <typename T>
	AVLTree<T>::~AVLTree()                               // deconstructor start // 
	{
		if(m_root == nullptr)                        // empty tree condition
			return;

		std::queue<Node*> queue;
		queue.push(m_root);                          // add root to the queue

		while(!queue.empty())                        // while queue is not empty
		{
			Node* current = queue.front();       

			if(current->left != nullptr)
				queue.push(current->left);   // add the current node's left child to the queue

			if(current->right != nullptr)
				queue.push(current->right);  // add the current nodes right child to the queue

			queue.pop();                         // pop the current element off the queue
			delete current;                      // delete the current element
		}
	}                                                    // deconstructor end //

	template <typename T>
	T* AVLTree<T>::insert(const T& newValue)                        // insert function start //
	{
		std::stack<Node*> stack{stackNodes(newValue)};

		if(stack.top() != nullptr)                              // if value already exists
			return &(stack.top()->value);                   // return pointer to its value

		stack.pop();                                            // top most node is nullptr so pop it off
		Node* parentNode = stack.top();                         // the parent node of new node is the top most node on the stack

		if(parentNode->value > newValue)                        // value is less than parent, making it the left child
			parentNode->left = new Node{newValue};

		else if(parentNode->value < newValue)                   // value is greater than the parent, making it the right child
			parentNode->right = new Node{newValue};

		unstackNodes(stack);                                    // update and balance nodes in the stack
		++m_size;                                               // increment the size
		return nullptr;                                         // return nullptr for a successful insertion
	}                                                               // insert function end //

	template <typename T>
	T AVLTree<T>::remove(const T& value)                                                                      // remove function start //
	{
		std::stack<Node*> stack{stackNodes(value)};

		Node* removingNode = stack.top();

		if(removingNode == nullptr)                                                                       // if value was not found, throw an error
			throw std::runtime_error{"AVLTree remove(), cannot remove value, value does not exist"};   
		
		T nodeValue = removingNode->value;                                                                // save the nodes value to return later

		if(removingNode->left == nullptr && removingNode->right == nullptr)                               // the node is a leaf node
			leafRemove(removingNode);                                                                 // remove the node

		else if(removingNode->left == nullptr || removingNode->right == nullptr)                          // the node has 1 subtree
			oneSubtreeRemove(removingNode);                                                           // remove the node
		
		else                                                                                              // the node has two subtrees
			twoSubtreeRemove(removingNode);                                                           // remove the node 

		--m_size;                                                                                         // decrement the size
		return nodeValue;                                                                                 // return the removed nodes value
	}                                                                                                         // remove function end //

	template <typename T>
	T* AVLTree<T>::find(const T& value)                      // find function start //
	{
		Node* currentNode = m_root;

		while(true)
		{
			if(currentNode == nullptr)     	         // nullptr condition 
				return nullptr;

			else if(currentNode->value > value)      // if value is less than currentNode's value go left
				currentNode = currentNode->left; // set the left child of the currentNode to currentNode

			else if(currentNode->value < value)      // if value is greater than currentNode's value go righ
				currentNode = currentNode->right;// set the right child of the currentNode to currentNode

			else                                     // if currentNode is not nullptr, not less, nor greater, it must be equal, so we found it 
				return &(currentNode->value);    // return pointer to the value
		}
	}                                                        // find function end //

	template <typename T>
	const T* AVLTree<T>::find(const T& value) const          // const find function start //
	{
		Node* currentNode = m_root;

		while(true)
		{
			if(currentNode == nullptr)     	         // nullptr condition 
				return nullptr;

			else if(currentNode->value > value)      // if value is less than currentNode's value go left
				currentNode = currentNode->left; // set the left child of the currentNode to currentNode

			else if(currentNode->value < value)      // if value is greater than currentNode's value go righ
				currentNode = currentNode->right;// set the right child of the currentNode to currentNode

			else                                     // if currentNode is not nullptr, not less, nor greater, it must be equal, so we found it 
				return &(currentNode->value);    // return pointer to the value
		}                                                 
	}                                                        // const find function end //

	template <typename T>
	bool AVLTree<T>::empty() const                           // empty function start //
	{
		return (m_root == nullptr && m_size == 0);       // if m_root is nullptr and size is 0, the tree is empty
	}                                                        // empty function end //
}

#endif
