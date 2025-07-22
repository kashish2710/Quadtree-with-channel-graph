#include <iostream>
#include <vector>
using namespace std;

struct Point {
    int x, y;
};

bool hasBend(const vector<Point>& line) {
    for (int i = 0; i + 2 < line.size(); i++) {
        int x1 = line[i+1].x - line[i].x;
        int y1 = line[i+1].y - line[i].y;
        int x2 = line[i+2].x - line[i+1].x;
        int y2 = line[i+2].y - line[i+1].y;

    
        if (x1 * y2 - y1 * x2 != 0) {
            return true;  // bend found
        }
    }
    return false;  // no bend
}

int main() {
    
    vector<Point> line = {
        {0,0}, {5,0}, {5,4}, {10,4}
    };

    if (hasBend(line)) {
        cout << "Line has a bend.\n";
    } else {
        cout << "Line is straight.\n";
    }
    return 0;
}
