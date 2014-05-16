#ifndef CPOINT_H
#define CPOINT_H

#include <string>
#include "ofMain.h"

using namespace std;

struct CPoint
{
 unsigned int x;
 unsigned int y;
};

CPoint getPoint(string s1, string s2);

#endif // CPOINT_H
