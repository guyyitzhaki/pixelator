#include "CPolygon.h"


CPolygon::CPolygon(bool temp) : CShape(temp)
{

}

CPolygon::~CPolygon()
{
    //dtor
}

void CPolygon::add(CPoint p) {
    points.push_back(p);
}

void CPolygon::pop() {
    points.pop_back();
}

string CPolygon::getType() {
    string s = "polygon";
    return s;
}

string CPolygon::getData() {
    string s = "";
    for (unsigned int i = 0; i < points.size(); i++)
    {
        s.append(ofToString(points[i].x));
        s.append(" ");
        s.append(ofToString(points[i].y));
        s.append(" ");
    }
    return s;
}

void CPolygon::draw() {
    ofBeginShape();
    for (unsigned int i = 0; i < points.size(); i++)
    {
        ofVertex(points[i].x, points[i].y);
    }
    ofEndShape();
}





