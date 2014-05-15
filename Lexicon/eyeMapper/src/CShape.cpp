
#include "CShape.h"


unsigned int globalId = 0;

CShape::CShape(bool temp)
{
    id = globalId;
    if (!temp)
        globalId++;
    mask = false;

}

CShape::~CShape()
{
    //dtor
}

string CShape::serialize() {
    string s = "";
    s.append(ofToString(getId()));
    s.append(" ");
    s.append(getType());
    s.append(" ");
    s.append(isMask() ? "true" : "false");
    s.append(" ");
    s.append(getData());
    return s;

}







