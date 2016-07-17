//
// Set (Map) based on Red-Black Tree
//
#ifndef TREESET_H
#define TREESET_H

#include "RBTree.h"

// An ABSTRACT class representing a key in TreeSet
class TreeSetKey {
public:
    TreeSetKey() {}
    virtual ~TreeSetKey() {}    // virtual destructor

    // Abstract class has virtual methods unimplemented

    // The keys can be compared
    virtual int compareTo(const TreeSetKey&) const = 0;

    bool operator==(const TreeSetKey& k) const {
        return (compareTo(k) == 0);
    }
    bool operator!=(const TreeSetKey& k) const {
        return (compareTo(k) != 0);
    }
    bool operator<(const TreeSetKey& k) const {
        return (compareTo(k) < 0);
    }
    bool operator<=(const TreeSetKey& k) const {
        return (compareTo(k) <= 0);
    }
    bool operator>(const TreeSetKey& k) const {
        return (compareTo(k) > 0);
    }
    bool operator>=(const TreeSetKey& k) const {
        return (compareTo(k) >= 0);
    }

    // The virtual method "clone" must allocate a copy of the object
    // in the dynamic memory. In any derived class Foo it must
    // be always implemented in the following way:
    // virtual Foo* clone() const { return new Foo(*this); }
    //
    virtual TreeSetKey* clone() const = 0;
};

// An ABSTRACT class representing a value of a key in TreeSet
class TreeSetValue {
public:
    TreeSetValue() {}
    virtual ~TreeSetValue() {}
    virtual TreeSetValue* clone() const = 0;
};

//
// class TreeSet implements "Map" or "Dictionary".
// It stores the set of pairs: (key, value).
// All keys are unique (different pairs have different keys).
//
// The implementation is based on the Red-Black Tree
//
class TreeSet: protected RBTree {
public:
    class Pair: public RBTreeNodeValue {
    public:
        const TreeSetKey* key;
        TreeSetValue* value;

        Pair():
            RBTreeNodeValue(),
            key(0),
            value(0)
        {}
        Pair(const TreeSetKey* k, TreeSetValue* v):
            RBTreeNodeValue(),
            key(k),
            value(v)
        {}

        virtual int compareTo(const RBTreeNodeValue& p) const {
            return key->compareTo(
                *((const Pair&) p).key
            );
        }
    };

    // Add a pair (key, value) to the set
    void add(const TreeSetKey* k, const TreeSetValue* v = 0);

    void remove(const TreeSetKey* key);

    // Return a value of a key
    TreeSetValue* value(const TreeSetKey* k) const;

    TreeSetValue* operator[](const TreeSetKey* k) const {
        return value(k);
    }

    bool contains(const TreeSetKey* k) const;

    int size() { return RBTree::size(); }

    class const_iterator: public RBTree::const_iterator {
    public:
        const_iterator(const RBTree::const_iterator& i):
            RBTree::const_iterator(i)
        {}

        const Pair& operator*() const { // Dereference
            return *((const Pair*) node->value);
        }
        const Pair* operator->() const {
            return &(operator*());
        }
    };

    class iterator: public RBTree::iterator {
    public:
        iterator(const RBTree::iterator& i):
            RBTree::iterator(i)
        {}

        Pair& operator*() const { // Dereference
            return *((Pair*) node->value);
        }
        Pair* operator->() const {
            return &(operator*());
        }
    };

    const_iterator begin() const {
        return RBTree::begin();
    }
    const_iterator end() const {
        return RBTree::end();
    }

    iterator begin() {
        return RBTree::begin();
    }
    iterator end() {
        return RBTree::end();
    }
};

#endif
