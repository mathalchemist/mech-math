#include "TreeSet.h"

bool TreeSet::contains(const TreeSetKey* k) const {
    Pair p(k, 0);
    return find(&p);
}

void TreeSet::add(const TreeSetKey* k, const TreeSetValue* v /* = 0 */) {
    Pair key(k, 0);
    RBTreeNode* node;
    if (find(&key, root(), &node)) {
        // The key is already in the set
        Pair* p = (Pair*) node->value;
        delete p->value;            // Remove the old value
        if (v != 0) {
            p->value = v->clone();  // Assign the new value
        } else {
            p->value = 0;
        }
    } else {
        // Add the pair to the set
        Pair* p = new Pair(k->clone(), v->clone());
        insert(node, p);
    }
}

TreeSetValue* TreeSet::value(const TreeSetKey* k) const {
    Pair key(k, 0);
    RBTreeNode* node;
    if (find(&key, root(), &node)) {
        // The key is in the set
        Pair* p = (Pair*) node->value;
        return p->value;
    } else {
        // The key is not found
        return 0;
    }
}
