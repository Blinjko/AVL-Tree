#pragma once
#include <stack>
#include <queue>
#include <stdexcept>

namespace DataStructures
{
    template <class T>
    class AVLTree
    {
        public:
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

//        public:

        AVLTree();                                      // constructor
        AVLTree(const AVLTree<T>&) = delete;            // copy constructor disabled
        ~AVLTree();                                     // destructor

        T* insert(const T&);                            // insert an element into the tree, returns a pointer to an element if it already exists, otherwise returns nullptr
        T remove(const T&);                             // remove an element from the tree, returns the value removed, if it does not exist an exception is thrown

        T* find(const T&);                              // trys to find an element given a value, if found it returns a pointer to the element, if not returns nullptr
        const T* find(const T&) const;                  // const version of find

        bool empty() const;                             // returns true if the tree is empty, false if not
        std::size_t size() const { return m_size; }     // returns the size of the tree

        T* root();                                      // returns the root node pointer
        const T* root() const;                          // const version of root() 

        private:

        std::stack<Node*> stackNodes(const T& value);   // trys to find matching node given a value, but every node that is iterated through is added to a stack
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
        if(m_root == nullptr)                            // empty tree condition
            return;

        std::queue<Node*> queue;
        queue.push(m_root);                              // add root to the queue

        while(!queue.empty())                            // while queue is not empty
        {
            Node* current = queue.front();       

            if(current->left != nullptr)
                queue.push(current->left);               // add the current node's left child to the queue

            if(current->right != nullptr)
                queue.push(current->right);              // add the current nodes right child to the queue

            queue.pop();                                 // pop the current element off the queue
            delete current;                              // delete the current element
        }
    }                                                    // deconstructor end //

    template <typename T>
    T* AVLTree<T>::insert(const T& newValue)                    // insert function start //
    {
        if(m_root == nullptr)                                   // empty tree condition
        {
            m_root = new Node{newValue};                        // set the root to the new node
            return nullptr;                                     // return nullptr for successful insertion
        }

        std::stack<Node*> stack{stackNodes(newValue)};

        if(stack.top() != nullptr)                              // if value already exists
            return &(stack.top()->value);                       // return pointer to its value

        stack.pop();                                            // top most node is nullptr so pop it off
        Node* parentNode = stack.top();                         // the parent node of new node is the top most node on the stack

        if(parentNode->value > newValue)                        // value is less than parent, making it the left child
        {
            parentNode->left = new Node{newValue};
            parentNode->left->parent = parentNode;              // set the child's parent to parentNode
        }

        else if(parentNode->value < newValue)                   // value is greater than the parent, making it the right child
        {
            parentNode->right = new Node{newValue};
            parentNode->right->parent = parentNode;             // set the child's parent to parentNode
        }

        unstackNodes(stack);                                    // update and balance nodes in the stack
        ++m_size;                                               // increment the size
        return nullptr;                                         // return nullptr for a successful insertion
    }                                                           // insert function end //

    template <typename T>
    T AVLTree<T>::remove(const T& value)                                           // remove function start //
    {
        std::stack<Node*> stack{stackNodes(value)};

        Node* removingNode = stack.top();

        if(removingNode == nullptr)                                                // if value was not found, throw an error
            throw std::runtime_error{
                "AVLTree remove(), cannot remove value, value does not exist"};   

        
        T nodeValue = removingNode->value;                                         // save the nodes value to return later

        if(removingNode->left == nullptr && removingNode->right == nullptr)        // the node is a leaf node
            leafRemove(removingNode);                                              // remove the node

        else if(removingNode->left == nullptr || removingNode->right == nullptr)   // the node has 1 subtree
            oneSubtreeRemove(removingNode);                                        // remove the node
        
        else                                                                       // the node has two subtrees
            twoSubtreeRemove(removingNode);                                        // remove the node 

        --m_size;                                                                  // decrement the size
        return nodeValue;                                                          // return the removed nodes value
    }                                                                              // remove function end //

    template <typename T>
    T* AVLTree<T>::find(const T& value)               // find function start //
    {
        Node* currentNode = m_root;

        while(true)
        {
            if(currentNode == nullptr)               // nullptr condition 
                return nullptr;

            else if(currentNode->value > value)      // if value is less than currentNode's value go left
                currentNode = currentNode->left;     // set the left child of the currentNode to currentNode

            else if(currentNode->value < value)      // if value is greater than currentNode's value go righ
                currentNode = currentNode->right;    // set the right child of the currentNode to currentNode

            else                                     // if currentNode is not nullptr, not less, nor greater, it must be equal, so we found it 
                return &(currentNode->value);        // return pointer to the value
        }
    }                                                // find function end //

    template <typename T>
    const T* AVLTree<T>::find(const T& value) const  // const find function start //
    {
        Node* currentNode = m_root;

        while(true)
        {
            if(currentNode == nullptr)               // nullptr condition 
                return nullptr;

            else if(currentNode->value > value)      // if value is less than currentNode's value go left
                currentNode = currentNode->left;     // set the left child of the currentNode to currentNode

            else if(currentNode->value < value)      // if value is greater than currentNode's value go righ
                currentNode = currentNode->right;    // set the right child of the currentNode to currentNode

            else                                     // if currentNode is not nullptr, not less, nor greater, it must be equal, so we found it 
                return &(currentNode->value);        // return pointer to the value
        }                                                 
    }                                                // const find function end //

    template <typename T>
    bool AVLTree<T>::empty() const                   // empty function start //
    {
        return (m_root == nullptr && m_size == 0);   // if m_root is nullptr and size is 0, the tree is empty
    }                                               // empty function end //

    template <typename T>
    T* AVLTree<T>::root()         // root function start //
    {
        if(m_root == nullptr)     // if root is a nullptr
            return nullptr;       // return nullptr
        return &(m_root->value);  // otherwise return a pointer to the root node's Value
    }                             // root function end // 

    template <typename T>
    const T* AVLTree<T>::root() const  // const root function start //
    {
        if(m_root == nullptr)          // if root is nullptr
            return nullptr;            // return nullptr
        return &(m_root->value);       // otherwise return a pointer to the rood node's value
    }                                  // end of const root function //

    template <typename T>
    std::stack<typename AVLTree<T>::Node*> AVLTree<T>::stackNodes(const T& value) 
    {                                                 // stackNodes function start //
        std::stack<Node*> stack;                      // create a stack of Node*
        stack.push(m_root);                           // add the root Node to the stack

        while(true)
        {
            Node* currentNode = stack.top();          // make a variable for easier readability, stores top most node on the stack, changes every loop iteration

            if(currentNode == nullptr)                // if the currentNode is nullptr, return the stack
                return stack;

            else if(currentNode->value > value)       // if the value of the currentNode is greater than the passed value
                stack.push(currentNode->left);        // then add the currentNodes left child to the stack

            else if(currentNode->value < value)       // if the value of the currentNode is less than than the passed value
                stack.push(currentNode->right);       // then add the currentNodes right child to the stack

            else                                      // this would only execute if the value of currentNode and the passed value are equal
                return stack;                         // if currentNode->value == value return the stack
        }
    }                                                 // stackNodes function end //

    template <typename T>
    void AVLTree<T>::unstackNodes(std::stack<Node*>& stack) // unstackNodes function start //
    {
        while(!stack.empty())                               // while the stack is not empty 
        {
            Node* currentNode = stack.top();                // created currentNode variable which holds the stacks current top most node

            if(currentNode == nullptr)                      // if currentNode == nullptr pop it off the stack
                stack.pop();

            else                                            // if currentNode is an actual node, then update and balance it, then pop it off
            {
                update(currentNode);
                balance(currentNode);
                stack.pop();
            }

        }
    }                                                       // unstackNodes function end //

    template <typename T>
    void AVLTree<T>::update(Node* node)                        // update function start //
    {
        if(node == nullptr)                                    // if passed value is nullptr return
            return;

        int leftHeight = -1;
        int rightHeight = -1;

        if(node->left != nullptr)                               // if the node has a left child get its height
            leftHeight = node->left->height;

        if(node->right != nullptr)                              // if the node has a right child get its height
            rightHeight = node->right->height;
       
        if(rightHeight >= leftHeight)                           // if rightHeight is greater than leftHeight set the node's height to rightHeight+1
            node->height = rightHeight+1;

        else if(rightHeight < leftHeight)                       // if rightHeight is less than leftHeight set the node's height to the leftHeight+1
            node->height = leftHeight+1;

        node->balanceFactor = (rightHeight+1) - (leftHeight+1); // calculate the balance factor, rightHeight+1 - leftHeight+1
    }                                                           // update function end //

    template <typename T>
    void AVLTree<T>::balance(Node* node)           // balance function start //
    {
        if(node == nullptr)                        // nullptr condition to avoid any segmentatio faults
            return;

        if(node->balanceFactor == -2)              // tree is left heavy
        {
            if(node->left->balanceFactor == 1)     // left right case
                leftRotation(node->left);          // left rotation on the passed node's left child

            rightRotation(node);                   // right rotation on the node
        }

        else if(node->balanceFactor == 2)          // tree is right heavy
        {
            if(node->right->balanceFactor == -1)   // right left case
                rightRotation(node->right);        // do a right rotation with the passed node's right child

            leftRotation(node);                    // do right rotation on the node
        }
    }                                              // balance function end // 

    template <typename T>
    void AVLTree<T>::rightRotation(Node* A) // rightRotation function start //
    {
        if(A == nullptr)                    // if passed node is nullptr return
            return;

        else if(A->left == nullptr)         // if passed node does not have a left child return
            return;

        Node* B = A->left;                  // make a variable B, A's current left child

        A->left = B->right;                 // B's right child becomes A's left child
        if(B->right != nullptr)             // if B's right child is not null makes its parent A
            B->right->parent = A;

        B->right = A;                       // Make B's right child A 
        B->parent = A->parent;              // B's parent is now A's parent

        if(B->parent == nullptr)            // if B's new parent is nullptr B is now the root node
            m_root = B;

        else if(B->parent->left == A)       // if B's parent's left child was A
            B->parent->left = B;            // make B the left child

        else if(B->parent->right == A)      // if B's parent's right child was A
            B->parent->right = B;           // make B the right child


        A->parent = B;                      // A's parent is now B

        update(A);                          // update A & B
        update(B);
    }

    template <typename T>
    void AVLTree<T>::leftRotation(Node* A)  // leftRotation function start //
    {
        if(A == nullptr)                    // if passed a nullptr, return
            return;

        else if(A->right == nullptr)        // if passed node doesn't have a right child, return
            return;

        Node* B = A->right;                 // create a variable B, A's right child
        
        A->right = B->left;                 // B's left child is now A's right child
        
        if(B->left != nullptr)              // if B's left child is not nullptr, set its parent to A
            B->left->parent = A;

        B->left = A;                        // make B's left child A
        B->parent = A->parent;              // make B's parent A's parent
    
        if(B->parent == nullptr)            // if B's parent is nullptr, make B the root node
            m_root = B;
        
        else if(B->parent->left == A)       // if B's parent's left child == A
            B->parent->left = B;            // make B's parent's left child B

        else if(B->parent->right == A)      // if B's parent's right child == A
            B->parent->right = B;           // make B's parent's right child B

        A->parent = B;                      // make A's parent B

        update(A);                          // update A & B
        update(B);

    }
}
