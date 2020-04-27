#ifndef AVLTREE_H
#define AVLTREE_H

#include <stack>
#include <queue>

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
	AVLTree<T>::AVLTree() : m_size{0}, m_root{nullptr}      
	{}

	template <typename T>
	AVLTree<T>::~AVLTree()                                  
	{
		if(m_root == nullptr)                        // empty tree condition
			return;

		typedef AVLTree<T>::Node Node; 
		std::queue<Node*> queue;
		queue.push(m_root);                          // add root to the queue

		while(!queue.empty())                        // while queue is not empty
		{
			Node* current = queue.front();       

			if(current.left != nullptr)
				queue.push(current.left);    // add the current node's left child to the queue

			if(current.right != nullptr)
				queue.push(current.right);   // add the current nodes right child to the queue

			queue.pop();                         // pop the current element off the queue
			delete current;                      // delete the current element
		}
	}

}

#endif
