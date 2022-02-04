#pragma once
#include <iostream>
#include <vector>

// ANSI Coloring, used for fancy print
#define ANSI_RED_TEXT "\033[31;1m"
#define ANSI_BLACK_TEXT "\033[37;1m"
#define ANSI_RESET "\033[0m"

/**
 * @brief Node class, holds a integer value
 * 
 */
class Node
{
public:
    int data;
    Node *parent, *right, *left;
    bool black;

    /**
     * @brief Construct a new Node using the given value
     * 
     * @param value Value to store in the node
     */
    Node(int value)
    {
        data = value;
        parent = nullptr;
        right = nullptr;
        left = nullptr;
        black = false;
    }

    /**
     * @brief Returns true if this current node is black according to the Red Black Tree rules. Handles null
     * 
     * @return true Is black
     * @return false Is red
     */
    bool isBlack()
    {
        if (this == nullptr)
            return true;
        return this->black;
    }

    /**
     * @brief Returns the grandparent of this node. 
     * 
     * @return Node* Grandparent of this node. nullptr if nonexistent
     */
    Node* grandparent()
    {
        if (this == nullptr || this->parent == nullptr)
            return nullptr;

        return this->parent->parent;
    }

    /**
     * @brief Returns true if this node is a left child relatively to its parent. Does not handle null
     * 
     * @return true Is a left child
     * @return false Is a right child
     */
    bool isLeftChild()
    {   
        return this->parent->left == this;
    }

    /**
     * @brief Returns this nodes sibling if it exists
     * 
     * @return Node* This node's sibling. nullptr if it does not exists
     */
    Node* sibling()
    {
        if (this == nullptr || this->parent == nullptr)
            return nullptr;

        if (this->isLeftChild())
            return this->parent->right;
        return this->parent->left;
    }

    /**
     * @brief Returns this node's uncle if it exists
     * 
     * @return Node* This node's uncle. nullptr if it does not exists
     */
    Node* uncle()
    {
        Node* grandparent = this->grandparent();
        if (grandparent == nullptr)
            return nullptr;
        
        if (this->parent->isLeftChild())
            return grandparent->right;
        return grandparent->left;
    }
};

class RBTree
{
public:
    bool contains(int value);
    void insert(int value);
    void remove(int value);
    void print();
    void inOrderPrint();
    void preOrderPrint();
    void postOrderPrint();

    std::vector<int> inOrderVec();
    std::vector<int> preOrderVec();
    std::vector<int> postOrderVec();

private:
    Node* root = nullptr;

    void insert(Node *node);
    void insertFix(Node *node);
    void leftRotate(Node *node);
    void rightRotate(Node *node);

    void remove(Node *node);
    void removeFix(Node *node);
    Node* successor(Node *node);
    Node* search(int value);

    void print(Node *current, std::string prefix, bool isLeft);
    void inOrderPrint(Node *node);
    void preOrderPrint(Node *node);
    void postOrderPrint(Node *node);

    std::vector<int> inOrderAsArray(Node *node, std::vector<int> *values);
    std::vector<int> postOrderAsArray(Node *node, std::vector<int> *values);
    std::vector<int> preOrderAsArray(Node *node, std::vector<int> *values);
};

/**
 * @brief Returns true if the given value is in the tree
 * 
 * @param value The value to search for
 * @return true The value is in the tree
 * @return false The value is not in the tree
 */
bool RBTree::contains(int value)
{
    return this->search(value) != nullptr;
}

/**
 * @brief Inserts the given value into the tree
 * 
 * @param value The value to insert
 */
void RBTree::insert(int value)
{
    Node *newNode = new Node(value);

    if (this->root == nullptr) // Root is empty. Add new value to root
    {
        this->root = newNode;
        this->root->black = true;
        return;
    }

    this->insert(newNode);
}

/**
 * @brief Inserts the given node into its appropiate position in the tree
 * 
 * @param node The node to insert
 */
void RBTree::insert(Node *node)
{
    Node *searcher = this->root, // Searches forward
         *tail = searcher; // Saves searcher's last position

    // Search for next empty correct position
    while (searcher != nullptr)
    {
        tail = searcher;
        if (node->data > searcher->data)
            searcher = searcher->right;
        else
            searcher = searcher->left;
    }

    // Choose right child according to size
    if (node->data > tail->data)
        tail->right = node;
    else
        tail->left = node;

    node->parent = tail;

    this->insertFix(node); // Fix any violations
}

/**
 * @brief Fixes any violations by iterating up the tree checking for uncle's color and flipping colours or rotating appropriately
 * 
 * @param node The node to start the fix process from
 */
void RBTree::insertFix(Node *node)
{
    // Check for double red
    while (!node->parent->isBlack())
    {
        if (node->parent->isLeftChild())
        {
            if (!node->uncle()->isBlack()) // If violated need to colour flip
            {
                node->parent->black = true;
                node->uncle()->black = true;
                node->grandparent()->black = false;
                node = node->grandparent();
            }
            else // If violated, Rotate. left left = right. Left right = left right
            {
                if (!node->isLeftChild())
                {
                    node = node->parent;
                    this->leftRotate(node);
                }

                node->parent->black = true;
                node->grandparent()->black = false;
                rightRotate(node->grandparent());
            }
        }
        else // same as above but handles right child
        {
            if (!node->uncle()->isBlack()) // If violated, need to colour flip
            {
                node->parent->black = true;
                node->grandparent()->left->black = true;
                node->grandparent()->black = false;
                node = node->grandparent();
            }
            else // If violated, Rotate. right right = left. right left = right left
            {
                if (node->isLeftChild())
                {
                    node = node->parent;
                    rightRotate(node);
                }

                node->parent->black = true;
                node->grandparent()->black = false;
                leftRotate(node->grandparent());
            }   
        }
    }
    this->root->black = true; // Root is always black
}

/**
 * @brief Do a left rotation on a given node
 * 
 * @param node Node to rotate
 */
void RBTree::leftRotate(Node *node)
{
    Node* child = node->right;

    node->right = child->left;
    if (child->left != nullptr)
        child->left->parent = node;

    child->parent = node->parent;
    if (child->parent == nullptr)
        this->root = child;
    else if (node->isLeftChild())
        node->parent->left = child;
    else
        node->parent->right = child;

    child->left = node;
    node->parent = child;
}

/**
 * @brief Do a right rotation on a given node
 * 
 * @param node Node to rotate
 */
void RBTree::rightRotate(Node *node)
{
    Node *child = node->left;

    node->left = child->right;
    if (child->right != nullptr)
        child->right->parent = node;

    child->parent = node->parent;
    if (child->parent == nullptr)
        this->root = child;
    else if (node->isLeftChild())
        node->parent->left = child;
    else
        node->parent->right = child;

    child->right = node;
    node->parent = child;
}

/**
 * @brief Remove a given value from the tree (THIS FEATURE IS BUGGY)
 * 
 * @param value The value to remove
 */
void RBTree::remove(int value)
{
    Node* toRemove = this->search(value);
    if (toRemove != nullptr)
        remove(toRemove);
}

/**
 * @brief Removes the given node from the tree (THIS FEATURE IS BUGGY)
 * 
 * @param node The node to remove
 */
void RBTree::remove(Node* node)
{
    Node* toBeRemoved = node;
    if (node->right == nullptr && node->left == nullptr) // is leaf
    {
        if (node != this->root)
        {
            // Removes the node and cut it from its parent
            if (node->isLeftChild())
                node->parent->left = nullptr;
            else
                node->parent->right = nullptr;
        }
        else
            root = nullptr;

        delete node;
    }
    else if (node->left != nullptr && node->right != nullptr) // two children
    {
        // Find best value to replace current and then transplant values and delete the node that was transplanted at the bottom
        Node *successor = this->successor(node->right);
        int successorsData = successor->data;
        remove(successor);
        node->data = successorsData;
    }
    else // only 1 child
    {
        // Deletes the value with respect to the nullptr child

        Node *child = nullptr;
        if (node->right != nullptr)
            child = node->right;
        else
            child = node->left;

        if (node != this->root)
        {
            if (node->isLeftChild())
                node->parent->left = child;
            else
                node->parent->right = child;
        }
        else
            this->root = child;

        delete node;
    }

    removeFix(node); // Fix violations (This breaks the tree :( )
}

/**
 * @brief Fixes any violations in the tree after a value removal by checking iterating upwards checking siblings and nephews and doing rotations and colour flips appropriately.
 * 
 * @param node The node which the deletion started
 */
void RBTree::removeFix(Node *node)
{
    Node* sibling;

    while (node != this->root && node->isBlack())
    {
        if (node->isLeftChild())
        {
            // Check sibling's colours and rotate and flip based on it and its relation

            sibling = node->sibling();

            if (!sibling->isBlack())
            {
                sibling->black = true;
                node->parent->black = false;
                leftRotate(node->parent);
                sibling = node->parent->right;
            }

            if (sibling != nullptr && sibling->left->isBlack() && sibling->right->isBlack())
            {
                sibling->black = false;
                node->parent->black = true;
                node = node->parent;
            }
            else
            {
                if (sibling != nullptr && sibling->right->isBlack())
                {
                    sibling->black = false;
                    sibling->left->black = true;
                    rightRotate(sibling);
                    sibling = node->parent->right;
                }

                if (sibling != nullptr)
                {
                    sibling->black = node->parent->black;
                    sibling->right->black = true;
                }
                node->parent->black = true;
                leftRotate(node->parent);
                node = this->root;
            }
        }
        else // Same as above but does it for the right child
        {
            sibling = node->sibling();

            if (!sibling->isBlack())
            {
                sibling->black = true;
                node->parent->black = true;
                rightRotate(node->parent);
                sibling = node->parent->left;
            }

            if (sibling != nullptr && sibling->left->isBlack() && sibling->right->isBlack())
            {
                sibling->black = false;
                sibling->right->black = true;
                leftRotate(sibling);
                sibling = node->parent->left;
            }

            if (sibling != nullptr) 
            {
                sibling->black = node->parent->black;
                sibling->left->black = true;
            }
            node->parent->black = true;
            rightRotate(node->parent);
            node = this->root;
        }
        
    }
    node->black = true;
}

/**
 * @brief Finds the in order successor of a given node
 * 
 * @param node The node to find successor for
 * @return Node* The successor
 */
Node* RBTree::successor(Node *node)
{
    while (node->left != nullptr)
        node = node->left;

    return node;
}

/**
 * @brief Searches the tree for a given a value and returns its node if it exists
 * 
 * @param value The value to search for
 * @return Node* The node containing the value. nullptr if it does not exists
 */
Node* RBTree::search(int value)
{
    Node *searcher = this->root;

    // Iterate downwards comparing until finding the value.
    while (searcher != nullptr)
    {   
        if (value > searcher->data)
            searcher = searcher->right;
        else if (value < searcher->data)
            searcher = searcher->left;
        else
            return searcher;
    }
    return nullptr; // not found
}

/**
 * @brief Recursivly prints the values IN ORDER
 * 
 * @param node Node for recrusion
 */
void RBTree::inOrderPrint(Node *node)
{
    if (node == nullptr)
        return;

    inOrderPrint(node->left);
    std::cout << node->data << " ";
    inOrderPrint(node->right);
}

/**
 * @brief Recursivly prints the values in PRE ORDER
 * 
 * @param node Node for recrusion
 */
void RBTree::preOrderPrint(Node *node)
{
    if (node == nullptr)
        return;
    
    std::cout << node->data << " ";
    preOrderPrint(node->left);
    preOrderPrint(node->right);
}

/**
 * @brief Recursivly prints the values in POST ORDER
 * 
 * @param node Node for recrusion
 */
void RBTree::postOrderPrint(Node *node)
{
    if (node == nullptr)
        return;
    
    postOrderPrint(node->left);
    postOrderPrint(node->right);
    std::cout << node->data << " ";
}

/**
 * @brief Recursivly prints the values IN ORDER
 * 
 */
void RBTree::inOrderPrint()
{
    std::cout << "IN ORDER:\t";
    inOrderPrint(this->root);
    std::cout << std::endl;
}

/**
 * @brief Recursivly prints the values in POST ORDER
 * 
 */
void RBTree::postOrderPrint()
{
    std::cout << "POST ORDER:\t";
    postOrderPrint(this->root);
    std::cout << std::endl;
}

/**
 * @brief Recursivly prints the values in PRE ORDER
 * 
 */
void RBTree::preOrderPrint()
{
    std::cout << "PRE ORDER:\t";
    preOrderPrint(this->root);
    std::cout << std::endl;
}

/**
 * @brief Recursively fill a returns a vector filled with the values of three in PRE ORDER
 * 
 * @param node Current node to add
 * @param values Vector of values
 * @return std::vector<int> Vector of values
 */
std::vector<int> RBTree::preOrderAsArray(Node *node, std::vector<int> *values)
{
    if (node == nullptr)
        return *values;
    
    values->push_back(node->data);
    preOrderAsArray(node->left, values);
    preOrderAsArray(node->right, values);

    return *values;
}

/**
 * @brief Recursively fill a returns a vector filled with the values of three IN ORDER
 * 
 * @param node Current node to add
 * @param values Vector of values
 * @return std::vector<int> Vector of values
 */
std::vector<int> RBTree::inOrderAsArray(Node *node, std::vector<int> *values)
{
    if (node == nullptr)
        return *values;
    
    inOrderAsArray(node->left, values);
    values->push_back(node->data);
    inOrderAsArray(node->right, values);

    return *values;
}

/**
 * @brief Recursively fill a returns a vector filled with the values of three in in POST ORDER
 * 
 * @param node Current node to add
 * @param values Vector of values
 * @return std::vector<int> Vector of values
 */
std::vector<int> RBTree::postOrderAsArray(Node *node, std::vector<int> *values)
{
    if (node == nullptr)
        return *values;
    
    postOrderAsArray(node->left, values);
    postOrderAsArray(node->right, values);
    values->push_back(node->data);

    return *values;
}

/**
 * @brief Recursively fill a returns a vector filled with the values of three IN ORDER
 * 
 * @return std::vector<int> IN ORDER vector
 */
std::vector<int> RBTree::inOrderVec()
{
    auto vec = std::vector<int>();
    return inOrderAsArray(this->root, &vec);
}

/**
 * @brief Recursively fill a returns a vector filled with the values of three in in PRE ORDER
 * 
 * @return std::vector<int> PRE ORDER vector
 */
std::vector<int> RBTree::preOrderVec()
{
    auto vec = std::vector<int>();
    return preOrderAsArray(this->root, &vec);
}

/**
 * @brief Recursively fill a returns a vector filled with the values of three in in POST ORDER
 * 
 * @return std::vector<int> POST ORDER vector
 */
std::vector<int> RBTree::postOrderVec()
{
    auto vec = std::vector<int>();
    return postOrderAsArray(this->root, &vec);
}


/**
 * @brief Recursivly prints the values in a fancy way
 * 
 */
void RBTree::print()
{
    this->print(this->root, "", false);
}

/**
 * @brief Recursivly prints the values in a fancy way (THIS METHOD IS NOT MINE. It most probably is from "VasiliNovikov")
 * 
 * @param current The current node to print and recurse from
 * @param prefix Prefix for the printing (Handles by the method)
 * @param isLeft If the node is left (Handled by the method)
 */
void RBTree::print(Node *current, std::string prefix, bool isLeft)
{
    if (current != nullptr)
    {
        std::cout << prefix << (isLeft ? "|---" : "\\---") << (current->black ? ANSI_BLACK_TEXT : ANSI_RED_TEXT) << current->data << ANSI_RESET << std::endl;
        print(current->left, prefix + (isLeft ? "|   " : "    "), true);
        print(current->right, prefix + (isLeft ? "|   " : "    "), false);
    }
}
