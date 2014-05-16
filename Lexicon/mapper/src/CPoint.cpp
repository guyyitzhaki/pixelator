#include "CPoint.h"

using namespace std;


CPoint getPoint(string s1, string s2) {
    CPoint p;
    p.x = ofToInt(s1);
    p.y = ofToInt(s2);
    return p;

}


