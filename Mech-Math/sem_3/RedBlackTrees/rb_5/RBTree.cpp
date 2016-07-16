// Red-Black Tree
// class RBTree, implementation
#include <assert.h>
#include "RBTree.h"

// Find a key in a subtree
// In: key  -- a key to find;
//     root -- a root of a subtree. If root == 0,
//             then find in complete tree
// Out: node -- a pointer to the node that contains a key,
//                  if key is in the set,
//              or a pointer to a node that should be parent to
//                  a node with this key in case of insertion
// Return value: true, if the subtree contains the key.
bool RBTree::find(
    const RBTreeNodeValue* key,
    const RBTreeNode* subTreeRoot /* = 0 */,
    RBTreeNodePtr* node /* = 0 */
) const {
    const RBTreeNode* x;        // current node
    const RBTreeNode* y;        // its parent
    if (subTreeRoot == 0) {
        x = root();
        y = &header;
    } else {
        x = subTreeRoot;
        y = x->parent;
    }
    while (x != 0) {
        const RBTreeNodeValue* currentKey = (const RBTreeNodeValue*) x->value;
        int n = key->compareTo(*currentKey);
        y = x;
        if (n == 0) {
            // key is found
            if (node != 0)
                *node = (RBTreeNode*) x;
            return true;
        } else if (n < 0) {
            x = x->left;
        } else {
            x = x->right;
        }
    }

    // key is not in the tree
    if (node != 0)
        *node = (RBTreeNode*) y;
    return false;
}

// Insert a key into the tree:
//     create a new node and insert it as a leaf.
// The color of a new node is red.
// Should be called after the "find" method, that returned "false".
void RBTree::insert(
    RBTreeNode* parentNode,
    RBTreeNodeValue* key
) {
    assert(parentNode != 0);
    int n = (-1);
    if (parentNode->value != 0)
        n = key->compareTo(*((const RBTreeNodeValue*) parentNode->value));
    assert(n != 0);
    RBTreeNode* x = new RBTreeNode();
    x->value = (void*) key;
    x->parent = parentNode;
    if (parentNode == &header)
        x->red = false;     // The root of tree is black
    else
        x->red = true;      // Paint the new node in red
    if (n < 0) {
        // Insert as a left son
        assert(parentNode->left == 0);
        parentNode->left = x;
    } else {
        // Insert as a right son
        assert(parentNode != &header && parentNode->right == 0);
        parentNode->right = x;
    }
    ++numNodes;

    if (x != root())
        rebalanceAfterInsert(x);
}

// Rotate a node x to the left    //
//        x                y      //
//       / \              / \     //
//      a   y    --->    x   c    //
//         / \          / \       //
//        b   c        a   b      //
void RBTree::rotateLeft(RBTreeNode* x) {
    RBTreeNode* y = x->right;
    assert(y != 0);
    RBTreeNode* p = x->parent;
    y->parent = p;
    if (x == p->left) {
        // x is the left son of its parent
        p->left = y;
    } else {
        // x is the right son of its parent
        p->right = y;
    }
    x->right = y->left;
    if (y->left != 0)
        y->left->parent = x;
    y->left = x;
    x->parent = y;
}

// Rotate a node x to the right   //
//        x                y      //
//       / \              / \     //
//      y   c    --->    a   x    //
//     / \                  / \   //
//    a   b                b   c  //
void RBTree::rotateRight(RBTreeNode* x) {
    RBTreeNode* y = x->left;
    assert(y != 0);
    RBTreeNode* p = x->parent;
    y->parent = p;
    if (x == p->left) {
        // x is the left son of its parent
        p->left = y;
    } else {
        // x is the right son of its parent
        p->right = y;
    }
    x->left = y->right;
    if (y->right != 0)
        y->right->parent = x;
    y->right = x;
    x->parent = y;
}

void RBTree::rebalanceAfterInsert(RBTreeNode* x) {
    assert(x->red);
    while(x != root() && x->parent->red) {
        if (x->parent == x->parent->parent->left) {
            // parent of x is a left son

            RBTreeNode* y = x->parent->parent->right; // y is the sibling of
                                             //            parent of x
            if (y != 0 && y->red) {          // if y is red
                x->parent->red = false;      //     color parent of x in black
                y->red = false;              //     color y in black
                x = x->parent->parent;       //     x = grandparent of x
                x->red = true;               //     color x in red
            } else {                         // else y is black
                if (x == x->parent->right) { //     if x is a right son
                    x = x->parent;           //         x = parent of x
                    rotateLeft(x);           //         left-rotate x
                }                            //     end if
                assert(x == x->parent->left);//     assert: x is a left son
                x->parent->red = false;      //     color parent of x in black
                x->parent->parent->red = true;  // color grandparent in red
                rotateRight(x->parent->parent); // right-rotate grandparent
            }                                // endif
        } else {
            // Mirror case: parent of x is a right son
            assert(x->parent == x->parent->parent->right);

            RBTreeNode* y = x->parent->parent->left; // y is the sibling of
                                             //           parent of x
            if (y != 0 && y->red) {          // if y is red
                x->parent->red = false;      //     color parent of x in black
                y->red = false;              //     color y in black
                x = x->parent->parent;       //     x = grandparent of x
                x->red = true;               //     color x in red
            } else {                         // else y is black
                if (x == x->parent->left) {  //     if x is a left son
                    x = x->parent;           //         x = parent of x
                    rotateRight(x);          //         right-rotate x
                }                            //     end if
                assert(x == x->parent->right); //   assert: x is a right son
                x->parent->red = false;      //     color parent of x in black
                x->parent->parent->red = true; //  color grandparent in red
                rotateLeft(x->parent->parent); //  left-rotate grandparent
            }                                // endif
        }
    } // end while

    // Always color the root in black
    if (x == root()) {
        x->red = false;
    }
}

void RBTree::eraseNode(RBTreeNode* node) {
    delete (RBTreeNodeValue*) node->value;
}

void RBTree::clear() {
    removeSubtree(root());
}

int RBTree::removeSubtree(RBTreeNode* subTreeRoot) {
    int numRemoved = 0;
    if (subTreeRoot == 0)
        return 0;
    if (subTreeRoot->left != 0)
        numRemoved += removeSubtree(subTreeRoot->left); // recursive call
    if (subTreeRoot->right != 0)
        numRemoved += removeSubtree(subTreeRoot->right); // recursive call

    if (subTreeRoot->parent->left == subTreeRoot)
        subTreeRoot->parent->left = 0;
    else
        subTreeRoot->parent->right = 0;

    eraseNode(subTreeRoot);
    delete subTreeRoot;
    ++numRemoved;
    --numNodes;

    assert(numNodes >= 0);

    return numRemoved;
}

const RBTreeNode* RBTree::minimalNode(
    const RBTreeNode* subTreeRoot /* = 0 */
) const {
    const RBTreeNode* x = subTreeRoot;
    if (x == 0)
        x = root();
    while (x != 0 && x->left != 0)
        x = x->left;
    return x;
}

const RBTreeNode* RBTree::maximalNode(
    const RBTreeNode* subTreeRoot /* = 0 */
) const {
    const RBTreeNode* x = subTreeRoot;
    if (x == 0)
        x = root();
    while (x != 0 && x->right != 0)
        x = x->right;
    return x;
}

const RBTreeNode* RBTree::nextNode(const RBTreeNode* node) const {
    assert(node != 0);
    if (node == &header)
        return minimalNode();

    if (node->right != 0) {
        return minimalNode(node->right);
    } else if (node == node->parent->left) { // node is a left son
        return node->parent;
    } else {                                 // node is a right son
        const RBTreeNode* x = node->parent;
        while (x == x->parent->right)        // while x is a right son
            x = x->parent;
        return x->parent;
    }
}

const RBTreeNode* RBTree::previousNode(const RBTreeNode* node) const {
    assert(node != 0);
    if (node == minimalNode())
        return &header;

    if (node->left != 0) {
        return maximalNode(node->left);
    } else if (node == node->parent->right) { // node is a right son
        return node->parent;
    } else {                                  // node is a left son
        const RBTreeNode* x = node->parent;
        while (x->parent != 0 && x == x->parent->left) // while x is a left son
            x = x->parent;
        if (x->parent != 0) {
            return x->parent;
        } else {
            assert(x == &header);
            return x;
        }
    }
}
