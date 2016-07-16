#include <iostream.h>
#include "R2Conv.h"

int main() {
    R2Point p;
    int i, n;

    cout << "Input number of points>";
    cin >> n;

    R2Convex conv;

    for (i = 0; i < n; i++) {
        cout << "Give the " << i <<"th point>";
        cin >> p.x >> p.y;
        conv.addPoint(p);
    }
    cout << "Perimeter = " << conv.perimeter() << endl
         << "Area = " << conv.area() << endl;
    return 0;
}
