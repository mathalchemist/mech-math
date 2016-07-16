#ifndef __H_INCLUDED__
#define __H_INCLUDED__

#include <string.h>
#include <math.h>

class TMatrix;
class TVector;

// TVector class
class TVector
{
public:
	double Coords[4];
	TVector(void);
	TVector(double X, double Y, double Z);
	TVector(const TVector& V);
	void Clear(void);
	void Set(double X, double Y, double Z);
	TVector& operator = (const TVector& V);
//	TVector operator + (const TVector& V) const;
//	TVector operator - (const TVector& V) const;
	TVector& operator += (const TVector& V);
	TVector& operator -= (const TVector& V);
	TVector& operator *= (double Value);
	TVector& operator *= (const TMatrix& M);
	TVector& operator /= (double Value);
	bool operator == (const TVector& V) const;
	bool operator != (const TVector& V) const;
	double Length(void) const;
	TVector& Normalize(void);
	void Add(double X, double Y, double Z);
	void Subtract(double X, double Y, double Z);
	void RotateX(double Angle);
	void RotateY(double Angle);
	void RotateZ(double Angle);
	void Rotate(TVector Dir, double Angle);
};

// TMatrix class
class TMatrix
{
public:
	double Elements[4][4];
	TMatrix(void);			// Empty matrix
	TMatrix(double Value);		// Init main diagonal with 'Value'
	TMatrix(const TMatrix& M);	// Init with matrix 'M'
	TMatrix(double E00, double E01, double E02, double E03,	// Init matrix with array of elements
			double E10, double E11, double E12, double E13,
			double E20, double E21, double E22, double E23,
			double E30, double E31, double E32, double E33);
	void Clear(void);	// Clear matrix
	void Clear(double Value);// Clear matrix & fill its main diagonal with 'Value'
	TMatrix& operator = (const TMatrix& M);
	TMatrix& operator *= (const TMatrix& M);
	void RotateX(double Angle);
	void RotateY(double Angle);
	void RotateZ(double Angle);
	void Rotate(TVector Dir, double Angle);
	void MoveTo(double X, double Y, double Z);// Absolute shift
	void MoveTo(TVector V);			// Absolute shift
	void Move(double X, double Y, double Z);// Relative shift
	void Move(TVector V);		// Relative shift
};

TVector operator + (const TVector& A, const TVector& B);
TVector operator - (const TVector& A, const TVector& B);
TVector operator * (double Value, const TVector& V);
TVector operator * (const TVector& V, double Value);
TVector operator & (const TVector& A, const TVector& B); // cross product
double operator | (const TVector& A, const TVector& B); // dot product
TVector operator * (const TVector& V, const TMatrix& M);
TVector operator / (const TVector& V, double Value);
TMatrix operator * (const TMatrix& A, const TMatrix& B);

#endif
