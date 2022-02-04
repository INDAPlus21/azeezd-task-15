#include "tree.hpp"
#include <iostream>
#include <cassert>

int main()
{
    int values[] = {5, 6, 4, 3, 1, 8, 9};
    int actualOrdered[] = {1, 3, 4, 5, 6, 8, 9};
    int actualPostOrd[] = {1, 4, 3, 6, 9, 8, 5};
    int actualPreOrd[] = {5, 3, 1, 4, 8, 6, 9};

    RBTree tree;

    for (auto &&i : values)
    {
        tree.insert(i);
        tree.print();

        tree.inOrderPrint();
        tree.preOrderPrint();
        tree.postOrderPrint();
    }

    // CHECK DIFFERENT TRAVERSALS
    auto inOrder = tree.inOrderVec();
    auto postOrder = tree.postOrderVec();
    auto preOrder = tree.preOrderVec();
    for (size_t i = 0; i < inOrder.size(); i++)
    {   
        assert(actualOrdered[i] == inOrder[i]);
        assert(actualPostOrd[i] == postOrder[i]);
        assert(actualPreOrd[i] == preOrder[i]);
    }
    
    
    // CHECK DELETIONS EXPECT WRONG
    assert(tree.contains(3) == true);
    tree.remove(3);
    assert(tree.contains(3) == false);
    tree.print();

    assert(tree.contains(1) == true);
    tree.remove(1);
    assert(tree.contains(1) == false);
    tree.print();

    std::cout << "OK!" << std::endl;

    return 0;
}