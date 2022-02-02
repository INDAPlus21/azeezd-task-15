#pragma once
#include <algorithm>
#include <iostream>

#define ANSI_RED_TEXT "\033[31;1m"
#define ANSI_BLACK_TEXT "\033[37;1m"
#define ANSI_RESET "\033[0m"

template <typename T>
class Node
{
public:
    T data;
    Node *left, *right, *parent;
    bool black;

    Node(T val, struct Node *parent = nullptr)
    {
        this->data = val;
        this->left = right = parent = nullptr;
        this->black = false;
    }

    Node *grandparent()
    {
        if (this->parent == nullptr)
        {
            return nullptr;
        }
        return this->parent->parent;
    }

    void setLeft(struct Node *left)
    {
        left->parent = this;
        this->left = left;
    }

    void setRight(Node *right)
    {
        right->parent = this;
        this->right = right;
    }

    static bool isBlack(Node<T> *node)
    {
        if (node == nullptr)
            return true;
        
        return node->black;
    }
};

template <typename T>
class RBTree
{
    // ========================= PUBLIC =========================
public:
    /**
     * @brief Construct a new Red-Black Tree object
     *
     */
    RBTree() { this->root = nullptr; };

    /**
     * @brief Inserts a new item into the red-black tree
     *
     * @tparam T A generic type that must form a total order
     * @param data The value of the data
     */
    void insert(T data)
    {
        Node<T> *newNode = new Node<T>(data);
        if (this->root == nullptr)
        {
            this->root = newNode;
            this->root->black = true;
            return;
        }

        this->insert(this->root, newNode);
    }
    
    
    void remove(T data)
    {
        Node<T> *toRemove = this->search(data);

        if (toRemove == nullptr)
            return;

        Node<T> *successor;
        
        if (toRemove->right == nullptr || toRemove->left == nullptr)
            successor = toRemove;
        else
            successor = this->successor(toRemove);

        if (successor->parent->right == successor)
        {
            successor->parent->right = successor->left;
            if (successor->left != nullptr)
                successor->left->parent = successor->parent;
        }
        else
        {
            successor->parent->left = successor->right;
            if (successor->right != nullptr)
                successor->right->parent = successor->parent;
        }

        toRemove->data = successor->data;
        this->correctAfterDelete(successor);
        delete successor;
    }

    /**
     * @brief Determines the height (depth) of the tree
     *
     * @return unsigned long The height (depth) of the tree
     */
    unsigned long height()
    {
        if (this->root == nullptr) // empty tree has 0 height
            return 0;

        return height(this->root) - 1; // start height recursion
    }

    /**
     * @brief Recursively prints the tree in a tree structure with some colours
     * 
     */
    void print()
    {
        print(this->root, "", false);
    }

    // ========================= PRIVATE =========================
private:
    Node<T> *root;

    void print(Node<T> *node)
    {
        if (node == nullptr)
        {
            std::cout << "NULL\n";
            return;
        }
        std::cout << node->data << "\n";
        
        print(node->right);
        print(node->left);
    }

    /**
     * @brief Recursively inserts the node in its correct position in the tree
     *
     * @tparam T A generic type that must form a total order
     * @param root The possibly parent tree to the new node
     * @param newNode The newly created, to-be-placed node
     */
    void insert(Node<T> *root, Node<T> *newNode)
    {
        if (newNode->data < root->data)
        {
            if (root->right == nullptr)
            {
                root->right = newNode;
                newNode->parent = root;
            }
            else
            {
                insert(root->right, newNode);
            }
        }
        else
        {
            if (root->left == nullptr)
            {
                root->left = newNode;
                newNode->parent = root;
            }
            else
            {
                insert(root->left, newNode);
            }
        }

        checkColor(newNode);
    }

    /**
     * @brief Recursively checks and corrects and colour violation
     *
     * @tparam T A generic type that must form a total order
     * @param node The node at which this method checks if any violation are caused
     */
    void checkColor(Node<T> *node)
    {
        if (node == this->root)
        {
            root->black = true;
            return;
        }

        if (node->grandparent() != nullptr && !node->black && !node->parent->black)
            correctTree(node);

        checkColor(node->parent);
    }

    /**
     * @brief Corrects the tree by either doing a colour flip or rotation, all based on the uncle's colour
     *
     * @tparam T A generic type that must form a total order
     * @param node The node at which a violation is to be corrected
     */
    void correctTree(Node<T> *node)
    {
        Node<T> *grandparent = node->grandparent();
        Node<T> *uncle;
        if (node->parent == grandparent->left)
            uncle = grandparent->right;
        else
            uncle = grandparent->left;

        if (uncle == nullptr || uncle->black) // null counts as black
            rotate(node);
        else if (uncle != nullptr)
            uncle->black = true;

        grandparent->black = false;
        node->parent->black = true;
    }

    /**
     * @brief Rotates the tree based on a given node's and its relative's relations
     *
     * @tparam T A generic type that must form a total order
     * @param node The node at which a violation is fixed through rotation of some kind based on relations
     */
    void rotate(Node<T> *node)
    {
        Node<T> *grandparent = node->grandparent();
        bool parentIsLeftChild = (grandparent->left == node->parent);

        if (node->parent->left == node)
        {
            if (parentIsLeftChild) // Violation in left left child
            {
                rightRotate(grandparent);
                node->black = false;
                node->parent->black = true;
                if (node->parent->right != nullptr)
                    node->parent->right->black = false;

                return;
            }
            // Violation in left right child
            rightLeftRotate(grandparent);
            node->black = true;
            node->right->black = false;
            node->left->black = false;
            return;
        }
        else
        {
            if (!parentIsLeftChild) // Violation in right right child
            {
                leftRotate(grandparent);
                node->black = false;
                node->parent->black = true;
                if (node->parent->right != nullptr)
                    node->parent->right->black = false;

                return;
            }
            // Violation in left right child
            leftRightRotate(grandparent);
            node->black = true;
            node->right->black = false;
            node->left->black = false;
            return;
        }
    }

    /**
     * @brief Rotates a node with a left rotation, making its left child the median. Fixes a violation
     *
     * @param node The grandparent of the node that caused a violation
     */
    void leftRotate(Node<T> *node)
    {
        Node<T> *temp = node->right;
        node->right = temp->left;
        if (node->right != nullptr)
            node->right->parent = node;

        if (node->parent == nullptr) // node is root
        {
            this->root = temp;
            temp->parent = nullptr;
        }
        else
        {
            temp->parent = node->parent;
            if (node->parent->left = node) // node is left child
                temp->parent->left = temp;
            else
                temp->parent->right = temp;
        }

        temp->left = node;
        node->parent = temp;
    }

    /**
     * @brief Rotates a node with a right rotation, making it right child the median. Fixes a violation
     *
     * @param node The grandparent of the node that caused a violation
     */
    void rightRotate(Node<T> *node)
    {
        Node<T> *temp = node->left;
        node->left = temp->right;
        if (node->left != nullptr)
            node->left->parent = node;

        if (node->parent == nullptr) // node is root
        {
            this->root = temp;
            temp->parent = nullptr;
        }
        else
        {
            temp->parent = node->parent;
            if (node->parent->left = node) // node is left child
                temp->parent->left = temp;
            else
                temp->parent->right = temp;
        }

        temp->right = node;
        node->parent = temp;
    }

    /**
     * @brief Rotates a node's left child to the left then rotates the node with a right rotation to fix a violation
     *
     * @param node The grandparent of the node that caused a violation
     */
    void leftRightRotate(Node<T> *node)
    {
        leftRotate(node->left); // Make violator to median node
        rightRotate(node);      // Rotate to make Median the parent
    }

    /**
     * @brief Rotates a node's right child with a right rotation then rotates the node with a left rotation to fix a violation
     *
     * @param node The grandparent of the node that caused a violation
     */
    void rightLeftRotate(Node<T> *node)
    {
        rightRotate(node->right); // Make violator to median node
        leftRotate(node);         // Rotate to make Median the parent
    }

    /**
     * @brief Takes a node and recursively determines the height of its subtrees
     *
     * @param node The node whose subtrees height is to be returned
     * @return unsigned long The height of a given nodes subtrees
     */
    unsigned long height(Node<T> *node)
    {
        if (node == nullptr) // reached an end
            return 0;

        return std::max( // Find height of left and right subtree and return which is bigger
            height(node->left) + 1,
            height(node->right) + 1);
    }

    /**
     * @brief Returns a pointer to the node containing a given value. nullptr if not found
     * 
     * @param value The value to search for
     * @return Node<T>* Pointer to a node with the given value.
     */
    Node<T>* search(T value)
    {
        Node<T> *root = this->root;

        while (root != nullptr)
        {
            if (value < root->data)
                root = root->right;
            else if (value > root->data)
                root = root->left; 
            else
                return root;
        }

        return nullptr;
    }

    /**
     * @brief Finds the the successor to a given node. A node that can replace this node maintaining the order
     * 
     * @param node The node to search the successor for
     * @return Node<T>* The successor for the given node
     */
    Node<T>* successor(Node<T> *node)
    {   
        if (node->right != nullptr)
        {
            node = node->right;
            while (node->left != nullptr)
                node = node->left;
            return node;
        }
        else if (node->left != nullptr)
        {
            node = node->left;
            while (node->right != nullptr)
                node = node->right;

            return node;
        }

        return nullptr;
    }

    /**
     * @brief Corrects any violation in the tree after deletion
     * 
     * @param node Node where the deletion happens
     */
    void correctAfterDelete(Node<T>* node)
    {
        // This should cover all the cases where violations might occur

        if (node == root) // Root reached
        {
            node->black = true;
            return;
        }

        Node<T> *sibling = nullptr;

        if (node->parent->right == node)
            sibling = node->parent->left;
        else 
            sibling = node->parent->right;

        if (!Node<T>::isBlack(sibling)) // Red sibling: fix through color change and rotation
        {
            sibling->black = true;
            node->parent->black = false;

            if (node->parent->left == node)
                leftRotate(node->parent);
            else
                rightRotate(node->parent);

            if (node->parent->right == node)
                sibling = node->parent->left;
            else 
                sibling = node->parent->right;
        }

        // Sibling is black with black children
        if (Node<T>::isBlack(sibling->left) && Node<T>::isBlack(sibling->right))
        {
            sibling->black = false;

            if (node->parent->black == false)
                node->parent->black = true;
            else
                correctAfterDelete(node->parent);
        }

        // Sibling is black with min 1 red child
        bool isLeftChild = node->parent->left == node;
        if (isLeftChild && Node<T>::isBlack(sibling->right))
        {
            sibling->left->black = true;
            sibling->black = false;
            rightRotate(sibling);
            sibling = node->parent->right;
        }
        else if (!isLeftChild && Node<T>::isBlack(sibling->left))
        {
            sibling->right->black = true;
            sibling->black = false;
            leftRotate(sibling);
            sibling = node->parent->left;
        }

        // Rotate parent appropiately and fix colours
        sibling->black = node->parent->black;
        node->parent->black = true;
        if (isLeftChild)
        {
            sibling->right->black = true;
            leftRotate(node->parent);
        }
        else
        {
            sibling->left->black = true;
            rightRotate(node->parent);
        }
    }

    /**
     * @brief Recursively prints the tree with "some" structure and colour included
     * 
     * @param node Current node to print
     * @param prefix Prefix of this node (depends on its position). Start with empty string, it will determinate what to use on its own
     * @param isLeft Boolean to tell the printer if this node is a left child or not. Changes the way it is printed
     */
    void print(Node<T> *node, const std::string &prefix, bool isLeft)
    {
        if (node != nullptr)
        {
            std::cout << prefix << (isLeft ? "|---" : "\\---") << (node->black ? ANSI_BLACK_TEXT : ANSI_RED_TEXT) << node->data << ANSI_RESET << std::endl;
            print(node->left, prefix + (isLeft ? "|   " : "    "), true);
            print(node->right, prefix + (isLeft ? "|   " : "    "), false);
        }
    }
};
