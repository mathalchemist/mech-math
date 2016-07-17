// Class String
// (c) rdmr, 2k6

#include <stdio.h>
#include <string.h>

class TString
{
    char * str;
    int len;
    int capacity;
public:
    TString(): str(0), len(0), capacity(256)
    {
	str = new char[capacity+1];
	str[0] = 0;
    }
    TString(const char * s): str(0), len(strlen(s))
    {
	capacity = len+256;
	str = new char[capacity+1];
	strcpy(str, s);
    }
    TString(const TString& s): str(0), len(s.len), capacity(s.capacity)
    {
	str = new char[capacity+1];
	strcpy(str, s.str);
    }
    ~TString() { delete[] str; }
    const char * c_str() const { return str; }    
    TString& operator = (const char * s)
    {
	int l = strlen(s);
	len = l;
	if (len > capacity)
	{
	    delete[] str;
	    capacity = len+256;
	    str = new char[capacity+1];	    
	}
	strcpy(str, s);
	return *this;
    }
    TString& operator = (const TString& s) { *this = s.c_str(); return *this; }
    TString& operator += (const char * s)
    {
	int l = strlen(s);
	len += l;
	if (len > capacity)
	{
	    capacity = len+256;
	    char * newstr = new char[capacity+1];
	    strcpy(newstr, str);
	    delete[] str;
	    str = newstr;
	}
	strcat(str, s);
	return *this;
    }
    TString& operator += (const TString& s) { *this += s.c_str(); return *this; }
    TString operator + (const char * s) const { TString R(*this); R += s; return R; }
    TString operator + (const TString& s) const { TString R(*this); R += s.c_str(); return R; }
    int Length() const { return len; }
    void FromInt(int n)
    {
	char s[256];
	sprintf(s, "%d", n);
	*this = s;
    }
    void FromDouble(double x)
    {
	char s[256];
	sprintf(s, "%.10lf", x);
	*this = s;
    }
    void Print(FILE * f = stdout) { fprintf(f, "%s\n", str); }
};

int main()
{
    TString s = "suxx";
    s += "Rulez;";
    s.Print();
    return 0;
}

