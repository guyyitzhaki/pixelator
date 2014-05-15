#ifndef CPOLYGON_H
#define CPOLYGON_H

#include <string>
#include "CPoint.h"
#include "CShape.h"
#include "ofMain.h"

using namespace std;

class CPolygon : public CShape
{
    public:
        CPolygon(bool temp);
        virtual ~CPolygon();
        void draw();
        void add(CPoint p);
        void pop();
        string getType();
        string getData();

    protected:
    private:
        vector<CPoint> points;
};

#endif // CPOLYGON_H
