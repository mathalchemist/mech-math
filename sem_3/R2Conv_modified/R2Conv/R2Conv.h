//
// File "R2Conv.h"
// Interface of class R2Convex
// Used classes:
//      deq of points (R2PointDeq),
//      R2Point, R2Vector

#ifndef CONV_H
#define CONV_H

#include <math.h>
#include "R2Graph/R2Graph.h"
#include "PointDeq.h"

class R2ConvexException
{
public:
    const char *reason;
    R2ConvexException(): reason("") {}
    R2ConvexException(const char *cause): reason(cause) {}
};

class R2Polygon
{
    R2PointDeq  m_Deq;
    double      m_Area;
    double      m_Perimeter;
    R2Vector 	m_MassCenter;
public:
    // Polygon begins with triangle
    R2Polygon(const R2Point& a, const R2Point& b, const R2Point& c);
    ~R2Polygon() {}
    double area() const { return m_Area; }
    double perimeter() const { return m_Perimeter; }
    R2Vector masscenter() const { return m_MassCenter; }
    void addPoint(const R2Point& t);
    void Rotate() { m_Deq.pushBack(m_Deq.popFront()); }
    int size() const { return m_Deq.size(); }

    class iterator: public R2PointDeq::iterator
    {
    public:
        iterator(): R2PointDeq::iterator() {}
        iterator(const R2PointDeq::iterator& i): R2PointDeq::iterator(i) {}
    };
    class const_iterator: public R2PointDeq::const_iterator
    {
    public:
        const_iterator(): R2PointDeq::const_iterator() {}
        const_iterator(const R2PointDeq::const_iterator& i): R2PointDeq::const_iterator(i) {}
        const_iterator(const iterator& i): R2PointDeq::const_iterator(i) {}
    };

    iterator begin() { return m_Deq.begin(); }
    const_iterator begin() const { return m_Deq.begin(); }
    iterator end() { return m_Deq.end(); }
    const_iterator end() const { return m_Deq.end(); }

    // Loop for each vertex of polygon
    // Return value: TRUE, if loop was broken
    // Input parameter:
    //      pointer to function with R2Point& parameter,
    //      this function is to be called for every point in deq;
    //      if this function returns true, than the loop will be
    //      broken.
    bool forEach(bool (*action)(R2Point&));
    bool IsInside(const R2Point& t);

private:
    R2Vector GetMassCenter() const;
    bool lit(const R2Point& a, const R2Point& b, const R2Point& t) const
    {
        double area = R2Point::signed_area(a, b, t);
        return (area > R2GRAPH_EPSILON || (fabs(area) <= R2GRAPH_EPSILON && !t.between(a, b)));
    }
    void remove(const R2Point& a, const R2Point& b, const R2Point& t);
};

// ==============================================================================================
class R2Convex {
    R2Point     m_A;            // First vertex of the line segment
    R2Point     m_B;            // Second vertex of line segment
    int         m_NumAng;       // Number of vertices: 0, 1, 2, many
    R2Polygon*  m_Polygon;      // Pointer to polygon
    R2Vector	m_MassCenter;

public:
    R2Convex(): m_A(), m_B(),  m_NumAng(0), m_Polygon(0), m_MassCenter(0,0) {}
    ~R2Convex() { delete m_Polygon; }
    double area() const
    {
        if (m_NumAng < 3) return 0.0;
        else return m_Polygon->area();
    }

    double perimeter() const
    {
        if (m_NumAng < 2) return 0.0;
        else
	    if (m_NumAng < 3) return 2. * (m_B - m_A).length();
    	    else return m_Polygon->perimeter();
    }
    R2Vector masscenter() const { return m_MassCenter; }
    // ADD POINT TO CONVEX HULL
    void addPoint(const R2Point& t); 
    int size() const { if (m_NumAng < 3) return m_NumAng; else return m_Polygon->size(); }

    void initialize() { m_NumAng = 0; delete m_Polygon; m_Polygon = 0; }

    class iterator {
        friend class R2Convex;
        int current;
        R2Convex* conv;
        R2Polygon::iterator polygon_iterator;
    public:
        iterator(): current(0), conv(0), polygon_iterator() {}
        iterator(R2Convex* c, int pos): current(pos), conv(c), polygon_iterator() {}

        iterator operator ++ () // Prefix increment operator
	{ if (conv != 0) if (conv->m_NumAng < 3) ++current; else ++polygon_iterator; return *this; }
        iterator operator ++ (int) { iterator tmp = *this; operator++(); return *this; }
        R2Point& operator * () throw (R2ConvexException)
	{
            if (conv == 0) throw R2ConvexException("Zero pointer dereference");
            if (conv->m_NumAng < 3)
	    {
                if (current == 0) return conv->m_A;
                else if (current == 1) return conv->m_B;
                else throw R2ConvexException("Pointer out of bounds");
            }
	    else return *polygon_iterator;
        }
        bool operator == (const iterator& i) const
	{
            return ((conv == 0 && i.conv == 0) ||
                    (conv != 0 && conv == i.conv && (
                    (conv->m_NumAng < 3 && current == i.current) ||
                    (conv->m_NumAng >= 3 && polygon_iterator == i.polygon_iterator))));
        }
        bool operator != (const iterator& i) const { return !operator==(i); }
    };
    class const_iterator: public iterator
    {
    public:
        const_iterator(): iterator() {}
        const_iterator(const R2Convex* c, int pos): iterator((R2Convex*) c, pos) {}
        const_iterator(const iterator& i): iterator(i) {}
        const R2Point& operator*() const throw (R2ConvexException) { return ((iterator*) this)->operator*(); }
        const R2Point* operator->() const throw (R2ConvexException) { return &(((iterator*) this)->operator*()); }
    };

    iterator begin()
    {
        iterator res(this, 0);
        if (m_NumAng >= 3) { res.polygon_iterator = m_Polygon->begin(); }
        return res;
    }
    const_iterator begin() const
    {
        const_iterator res(this, 0);
        if (m_NumAng >= 3) res.polygon_iterator = ((const R2Polygon*) m_Polygon)->begin();
        return res;
    }

    iterator end()
    {
        iterator res(this, m_NumAng);
        if (m_NumAng >= 3) res.polygon_iterator = m_Polygon->end(); 
        return res;
    }
    const_iterator end() const
    {
        const_iterator res(this, m_NumAng);
        if (m_NumAng >= 3) res.polygon_iterator = ((const R2Polygon*) m_Polygon)->end(); 
        return res;
    }
    // Loop for each vertex of convex
    // Return value: true, if loop was broken
    // Input parameter:
    //      pointer to function with R2Point& parameter,
    //      this function is to be called for every point in deq;
    //      if this function returns true, than the loop will be
    //      broken.
    bool forEach(bool (*action)(R2Point&));
    bool IsInside(const R2Point& t);
};

#endif
//
// End of file "r2conv.h"
