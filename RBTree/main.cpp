#include "tree.hpp"
#include <iostream>

int main()
{
    RBTree<int> tree;

    for (size_t i = 0; i < 10; i++)
    {
        tree.insert(i);
    }
    
    tree.print();

    tree.remove(7);

    tree.print();

    return 0;
}