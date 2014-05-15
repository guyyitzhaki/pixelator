#include "CLine.h"


CLine::CLine(CPoint _p1, CPoint _p2, bool temp) : CShape(temp)
{

    p1 = _p1;
    p2 = _p2;
}

CLine::~CLine()
{
    //dtor
}

void CLine::draw() {
    ofLine(p1.x, p1.y, p2.x, p2.y);
}


string CLine::getType() {
    string s = "line";
    return s;
}

string CLine::getData() {
    string s = "";
    s.append(ofToString(p1.x));
    s.append(" ");
    s.append(ofToString(p1.y));
    s.append(" ");
    s.append(ofToString(p2.x));
    s.append(" ");
    s.append(ofToString(p2.y));
    return s;
}





