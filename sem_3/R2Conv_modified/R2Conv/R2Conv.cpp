//
// File "Conv.cpp"
// Implementation of classes Convex, Polygon
//
#include <stdlib.h>
#include <assert.h>
#include "R2Conv.h"

bool R2Polygon::IsInside(const R2Point& t)
{
    int Num = size();
    if (Num < 3) return false;
    for (int i = 0; i < Num; i++)
    {
        if (lit(m_Deq.back(), m_Deq.front(), t)) return false;
        Rotate();
    }
    return true;
}


R2Polygon::R2Polygon(const R2Point& a, const R2Point& b, const R2Point& c): m_Deq(), m_Area(0.), m_Perimeter(0.)
{
    assert(!R2Point::on_line(a, b, c));
    m_Deq.pushFront(b);
    if (lit(a, c, b))
    {
        m_Deq.pushFront(a);
        m_Deq.pushBack(c);
    }
    else
    {
        m_Deq.pushFront(c);
        m_Deq.pushBack(a);
    }

    assert(!lit(m_Deq.back(), m_Deq.front(), b));
    m_Perimeter = a.distance(b) + b.distance(c) + c.distance(a);
    m_Area = R2Point::area(a, b, c);
}

void R2Polygon::addPoint(const R2Point& t)
{
    int i; R2Point x;
    int num = m_Deq.size();    // Number of vertices
    // Try to find the lit edge
    for (i = 0; i < num; i++) { if (lit(m_Deq.back(), m_Deq.front(), t)) break; Rotate(); }
    if (!lit(m_Deq.back(), m_Deq.front(), t)) return;
    // Delete the current edge (i.e. modify m_Area and m_Perimeter)
    remove(m_Deq.back(), m_Deq.front(), t);
    // Delete lit edges from the beginning of deq
    x = m_Deq.popFront();
    while (lit(x, m_Deq.front(), t))
    {
        remove(x, m_Deq.front(), t);
        x = m_Deq.popFront();
	
    }
    m_Deq.pushFront(x);;
    // Delete lit edges from the end of deq
    x = m_Deq.popBack();
    while (lit(m_Deq.back(), x, t))
    {
        remove(m_Deq.back(), x, t);
        x = m_Deq.popBack();
	
    }
    m_Deq.pushBack(x);
    m_Perimeter += m_Deq.front().distance(t) + t.distance(m_Deq.back());
    m_Deq.pushFront(t);
    m_MassCenter = GetMassCenter();
}

void R2Polygon::remove(const R2Point& a, const R2Point& b, const R2Point& t)
{
    assert(lit(a, b, t));   // Edge [a, b> is lit from the point t.
    m_Perimeter -= a.distance(b);
    m_Area += fabs(R2Point::area(a, b, t));
}

// Loop for every vertex of polygon
// Return value: TRUE, if loop was broken,
//               FALSE otherwise
// Input parameter:
//      pointer to function with R2Point& parameter,
//      this function is to be called for every point in deq;
//      if this function returns TRUE, than the loop will be
//      broken.
bool R2Polygon::forEach(bool (*action)(R2Point&))
{
    int n = m_Deq.size(); int i;
    for (i = 0; i < n; i++) { if ((*action)(m_Deq.front())) return true; Rotate(); }
    return false;
}

R2Vector R2Polygon::GetMassCenter() const
{
    R2Vector mc(0,0);
    for (R2Polygon::iterator i = begin(); i != end(); i++) mc += R2Vector(i->x, i->y);
    mc *= (1/double(size()));
    return mc;
}
// end of polygon
//========================================================

void R2Convex::addPoint(const R2Point& t)
{
    if (m_NumAng == 0)
    {
        m_NumAng++;
        m_A = t;
	m_MassCenter = R2Vector(t.x,t.y);
    }
    else if (m_NumAng == 1)
    {
        if (t != m_A)
	{
            m_NumAng++;
            m_B = t;
        }
	R2Point x = (m_A + m_B)*0.5; m_MassCenter = R2Vector(x.x,x.y);
    }
    else if (m_NumAng == 2)
    {
        if (!R2Point::on_line(m_A, m_B, t))
	{
            m_Polygon = new R2Polygon(m_A, m_B, t);
	    R2Point x = (m_A + m_B + t)*(1/3.0); m_MassCenter = R2Vector(x.x,x.y);
            m_NumAng++;
        }
	else
	{
	    if (m_B.between(m_A, t)) m_B = t; else if (m_A.between(t, m_B)) m_A = t; /* else leave them "as is" */
	    R2Point x = (m_A + m_B)*0.5; m_MassCenter = R2Vector(x.x,x.y);
	}
    }
    else
    {
        assert(m_NumAng >= 3);
        // Pass the request to polygon
        m_Polygon->addPoint(t);
	m_MassCenter = m_Polygon->masscenter();
    }
}

// Loop for every vertex of convex
// Return value: TRUE, if loop was broken,
//               FALSE otherwise
// Input parameter:
//      pointer to function with R2Point& parameter,
//      this function is to be called for every point in deq;
//      if this function returns true, than the loop will be
//      broken.
bool R2Convex::forEach(bool (*action)(R2Point&))
{
    if (m_NumAng == 0) return false;
    else if (m_NumAng == 1) { return (*action)(m_A); }
    else if (m_NumAng == 2)
    {
        if ((*action)(m_A)) return true;
        if ((*action)(m_B)) return true; else return false;
    }
    else return m_Polygon->forEach(action);
}

bool R2Convex::IsInside(const R2Point& t) { return m_Polygon->IsInside(t); }

// End of implementation of the class R2Convex
//======================================================

