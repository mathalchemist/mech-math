//
// Deq of points (Double Ended Queue)
// Used classes: R2Point
//

#ifndef POINT_DEQ_H
#define POINT_DEQ_H

#include "R2Graph/R2Graph.h"

class DeqException {
public:
    const char *reason;
    DeqException():
        reason("")
    {}

    DeqException(const char *cause):
        reason(cause)
    {}
};

const int DEQ_MAXELEM = 1024;

class R2PointDeq {
private:
    const int   m_MaxElem;
    int         m_Begin;
    int         m_End;
    int         m_NumElem;
    R2Point*    m_Elements;

public:
    R2PointDeq():
        m_MaxElem(DEQ_MAXELEM),
        m_Begin(0),
        m_End(DEQ_MAXELEM-1),
        m_NumElem(0),
        m_Elements(0)
    {
        m_Elements = new R2Point[m_MaxElem];
    }

    R2PointDeq(int maxElem):
        m_MaxElem(maxElem),
        m_Begin(0),
        m_End(maxElem-1),
        m_NumElem(0),
        m_Elements(0)
    {
        m_Elements = new R2Point[m_MaxElem];
    }

    ~R2PointDeq() { delete[] m_Elements; }

private:

    int nextIndex(int i) const {
        if (i < m_MaxElem-1) return i+1;
        else return 0;
    }

    int prevIndex(int i) const {
        if (i > 0) return i-1;
        else return m_MaxElem-1;
    }

public:
    void pushFront(const R2Point& p) throw (DeqException) {
        if (m_NumElem >= m_MaxElem)
            throw DeqException("Deq overflow");
        m_Begin = prevIndex(m_Begin);
        m_Elements[m_Begin] = p;
        m_NumElem++;
    }

    void pushBack(const R2Point& p) throw (DeqException) {
        if (m_NumElem >= m_MaxElem)
            throw DeqException("Deq overflow");
        m_End = nextIndex(m_End);
        m_Elements[m_End] = p;
        m_NumElem++;
    }

    R2Point popFront() throw (DeqException) {
        if (m_NumElem <= 0)
            throw DeqException("Deq empty");
        int i = m_Begin;
        m_Begin = nextIndex(m_Begin);
        m_NumElem--;
        return m_Elements[i];
    }

    R2Point popBack() throw (DeqException) {
        if (m_NumElem <= 0)
            throw DeqException("Deq empty");
        int i = m_End;
        m_End = prevIndex(m_End);
        m_NumElem--;
        return m_Elements[i];
    }

    R2Point& front() throw (DeqException) {
        if (m_NumElem <= 0)
            throw DeqException("Deq empty");
        return m_Elements[m_Begin];
    }

    R2Point& back() throw (DeqException) {
        if (m_NumElem <= 0)
            throw DeqException("Deq empty");
        return m_Elements[m_End];
    }

    void clear() {
        m_Begin = 0;
        m_End = m_MaxElem-1;
        m_NumElem = 0;
    }

    int size() const { return m_NumElem; }
    int maxSize() const { return m_MaxElem; }

    class iterator {
        R2PointDeq* deq;
        int current;
        int pos;
    public:
        iterator(): deq(0), current(0), pos(0) {}
        iterator(R2PointDeq* d, int firstElem, int firstPos): deq(d), current(firstElem), pos(firstPos) {}
        iterator(const iterator& i): deq(i.deq), current(i.current), pos(i.pos) {}
        iterator& operator++() { // Prefix increment operator
            pos++;
            current = deq->nextIndex(current);
            return *this;
        }

        iterator operator++(int) { // Postfix increment operator
            iterator tmp = *this;
            operator++(); // Apply the prefix increment operator
            return tmp;
        }

        R2Point& operator*() throw (DeqException) {
            if (pos >= deq->size())
                throw DeqException("Index out of bounds");
            return deq->m_Elements[current];
        }

        R2Point* operator->() throw (DeqException) {
            if (pos >= deq->size())
                throw DeqException("Index out of bounds");
            return &(deq->m_Elements[current]);
        }

        bool operator==(const iterator& i) const {
            return (deq == i.deq && current == i.current);
        }
        bool operator!=(const iterator& i) const {
            return !operator==(i);
        }
    };

    class const_iterator: public iterator {
    public:
        const_iterator():
            iterator()
        {}

        const_iterator(const R2PointDeq* d, int firstElem, int firstPos):
            iterator((R2PointDeq*) d, firstElem, firstPos) {}
        const_iterator(const iterator& i): iterator(i) {}
        const R2Point& operator*() const throw (DeqException)
	    { return ((iterator*) this)->operator*(); }
        const R2Point* operator->() const throw (DeqException)
	    { return &(((iterator*) this)->operator*()); }
    };

    iterator begin() { return iterator(this, m_Begin, 0); }
    const_iterator begin() const { return const_iterator(this, m_Begin, 0); }

    iterator end() { return iterator(this, nextIndex(m_End), size()); }

    const_iterator end() const { return const_iterator(this, nextIndex(m_End), size()); }
};

#endif
//
// End of file "PointDeq.h"
